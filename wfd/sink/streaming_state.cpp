/*
 * This file is part of wysiwidi
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "streaming_state.h"

#include "wfd/public/media_manager.h"

#include "cap_negotiation_state.h"
#include "wfd/parser/pause.h"
#include "wfd/parser/play.h"
#include "wfd/parser/reply.h"
#include "wfd/parser/teardown.h"
#include "wfd/parser/triggermethod.h"
#include "wfd/common/typed_message.h"
#include "wfd_session_state.h"

namespace wfd {

template <WFD::TriggerMethod::Method method>
class M5Handler final : public MessageReceiver<TypedMessage::M5> {
 public:
  explicit M5Handler(const InitParams& init_params)
    : MessageReceiver<TypedMessage::M5>(init_params) {
  }

  virtual bool CanHandle(TypedMessage* message) const override {
    if (!MessageReceiver<TypedMessage::M5>::CanHandle(message))
      return false;

    auto property =
      static_cast<WFD::TriggerMethod*>(message->payload().get_property(WFD::WFD_TRIGGER_METHOD).get());
    return  method == property->method();
  }

  virtual bool HandleMessage(std::unique_ptr<TypedMessage> message) override {
    WFD::Reply reply;
    reply.header().set_cseq(message->cseq());
    sender_->SendRTSPData(reply.to_string());
    return true;
  }
};

class M7Handler final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto play = std::make_shared<WFD::Play>(manager_->PresentationUrl());
    play->header().set_session(manager_->Session());
    play->header().set_cseq (send_cseq_++);
    return std::unique_ptr<M7>(new M7(play));
  }

  virtual bool HandleReply(Reply* reply) override {
    return (reply->GetResponseCode() == 200);
  }
};

class PlayHandler : public MessageSequenceHandler {
 public:
  explicit PlayHandler(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
    AddSequencedHandler(new M5Handler<WFD::TriggerMethod::PLAY>(init_params));
    AddSequencedHandler(new M7Handler(init_params));
  }
};

class M8Handler final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto teardown = std::make_shared<WFD::Teardown>(manager_->PresentationUrl());
    teardown->header().set_session(manager_->Session());
    teardown->header().set_cseq (send_cseq_++);
    return std::unique_ptr<M8>(new M8(teardown));
  }

  virtual bool HandleReply(Reply* reply) override {
    return (!manager_->Session().empty() && (reply->GetResponseCode() == 200));
  }
};

TeardownHandler::TeardownHandler(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
  AddSequencedHandler(new M5Handler<WFD::TriggerMethod::TEARDOWN>(init_params));
  AddSequencedHandler(new M8Handler(init_params));
}

class M9Handler final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto pause = std::make_shared<WFD::Pause>(manager_->PresentationUrl());
    pause->header().set_session(manager_->Session());
    pause->header().set_cseq (send_cseq_++);
    return std::unique_ptr<M9>(new M9(pause));
  }

  virtual bool HandleReply(Reply* reply) override {
    return (reply->GetResponseCode() == 200);
  }
};

class PauseHandler : public MessageSequenceHandler {
 public:
  explicit PauseHandler(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
    AddSequencedHandler(new M5Handler<WFD::TriggerMethod::PAUSE>(init_params));
    AddSequencedHandler(new M9Handler(init_params));
  }
};

StreamingState::StreamingState(const InitParams& init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(new TeardownHandler(init_params));
  AddOptionalHandler(new PlayHandler(init_params));
  AddOptionalHandler(new PauseHandler(init_params));
  AddOptionalHandler(new M3Handler(init_params));
  AddOptionalHandler(new M4Handler(init_params));
}

StreamingState::~StreamingState() {
}

}  // miracast
