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
#include "wfd_session_state.h"

namespace wfd {
namespace sink {

template <TriggerMethod::Method method>
class M5Handler final : public MessageReceiver<Request::M5> {
 public:
  explicit M5Handler(const InitParams& init_params)
    : MessageReceiver<Request::M5>(init_params) {
  }

  virtual bool CanHandle(Message* message) const override {
    if (!MessageReceiver<Request::M5>::CanHandle(message))
      return false;

    auto property =
      static_cast<TriggerMethod*>(message->payload().get_property(WFD_TRIGGER_METHOD).get());
    return  method == property->method();
  }

  virtual std::unique_ptr<Reply> HandleMessage(Message* message) override {
    return std::unique_ptr<Reply>(new Reply());
  }
};

class M7Sender final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<Message> CreateMessage() override {
    Play* play = new Play(ToSinkMediaManager(manager_)->PresentationUrl());
    play->header().set_session(ToSinkMediaManager(manager_)->Session());
    play->header().set_cseq (send_cseq_++);
    return std::unique_ptr<Message>(play);
  }

  virtual bool HandleReply(Reply* reply) override {
    if (reply->response_code() == 200) {
      manager_->Play();
      return true;
    }
    return false;
  }
};

class PlayHandler : public MessageSequenceHandler {
 public:
  explicit PlayHandler(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
    AddSequencedHandler(new M5Handler<TriggerMethod::PLAY>(init_params));
    AddSequencedHandler(new M7Sender(init_params));
  }
};

class M8Sender final : public SequencedMessageSender {
 public:
  using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<Message> CreateMessage() override {
    Teardown* teardown = new Teardown(ToSinkMediaManager(manager_)->PresentationUrl());
    teardown->header().set_session(ToSinkMediaManager(manager_)->Session());
    teardown->header().set_cseq (send_cseq_++);
    return std::unique_ptr<Message>(teardown);
  }

  virtual bool HandleReply(Reply* reply) override {
    if (!ToSinkMediaManager(manager_)->Session().empty() && (reply->response_code() == 200)) {
      manager_->Teardown();
      return true;
    }
    return false;
  }
};

TeardownHandler::TeardownHandler(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
  AddSequencedHandler(new M5Handler<wfd::TriggerMethod::TEARDOWN>(init_params));
  AddSequencedHandler(new M8Sender(init_params));
}

class M9Sender final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<Message> CreateMessage() override {
    Pause* pause = new Pause(ToSinkMediaManager(manager_)->PresentationUrl());
    pause->header().set_session(ToSinkMediaManager(manager_)->Session());
    pause->header().set_cseq (send_cseq_++);
    return std::unique_ptr<Message>(pause);
  }

  virtual bool HandleReply(Reply* reply) override {
    if (reply->response_code() == 200) {
      manager_->Pause();
      return true;
    }
    return false;
  }
};

class PauseHandler : public MessageSequenceHandler {
 public:
  explicit PauseHandler(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
    AddSequencedHandler(new M5Handler<TriggerMethod::PAUSE>(init_params));
    AddSequencedHandler(new M9Sender(init_params));
  }
};

class M7SenderOptional final : public OptionalMessageSender<Request::M7> {
 public:
  M7SenderOptional(const InitParams& init_params)
    : OptionalMessageSender<Request::M7>(init_params) {
  }
 private:
  virtual bool HandleReply(Reply* reply) override {
    if (reply->response_code() == 200) {
      manager_->Play();
      return true;
    }
    return false;
  }

  virtual bool CanSend(Message* message) const override {
    if (OptionalMessageSender<Request::M7>::CanSend(message)
        && manager_->IsPaused())
      return true;
    return false;
  }
};

class M8SenderOptional final : public OptionalMessageSender<Request::M8> {
 public:
  M8SenderOptional(const InitParams& init_params)
    : OptionalMessageSender<Request::M8>(init_params) {
  }
 private:
  virtual bool HandleReply(Reply* reply) override {
    // todo: if successfull, switch to init state
    if (reply->response_code() == 200) {
      manager_->Teardown();
      return true;
    }

    return false;
  }
};

class M9SenderOptional final : public OptionalMessageSender<Request::M9> {
 public:
  M9SenderOptional(const InitParams& init_params)
    : OptionalMessageSender<Request::M9>(init_params) {
  }
 private:
  virtual bool HandleReply(Reply* reply) override {
    if (reply->response_code() == 200) {
      manager_->Pause();
      return true;
    }
    return false;
  }

  virtual bool CanSend(Message* message) const override {
    if (OptionalMessageSender<Request::M9>::CanSend(message)
        && !manager_->IsPaused())
      return true;
    return false;
  }
};

StreamingState::StreamingState(const InitParams& init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(new TeardownHandler(init_params));
  AddOptionalHandler(new PlayHandler(init_params));
  AddOptionalHandler(new PauseHandler(init_params));
  AddOptionalHandler(new M3Handler(init_params));
  AddOptionalHandler(new M4Handler(init_params));

  // optional senders that handle sending play, pause and teardown
  AddOptionalHandler(new M7SenderOptional(init_params));
  AddOptionalHandler(new M8SenderOptional(init_params));
  AddOptionalHandler(new M9SenderOptional(init_params));
}

StreamingState::~StreamingState() {
}

}  // sink
}  // wfd
