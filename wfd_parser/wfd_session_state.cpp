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

#include "wfd_session_state.h"

#include "context_manager.h"

#include "cap_negotiation_state.h"
#include "reply.h"
#include "setparameter.h"
#include "triggermethod.h"
#include "typed_message.h"

namespace wfd {

class M5Handler final : public SequencedMessageSender {
 public:
  using SequencedMessageSender::SequencedMessageSender;

 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto set_param =
        std::make_shared<WFD::SetParameter>("rtsp://localhost/wfd1.0");
    set_param->header().set_cseq(send_cseq_++);
    set_param->payload().add_property(
        std::shared_ptr<WFD::Property>(new WFD::TriggerMethod(WFD::TriggerMethod::SETUP)));
    return std::unique_ptr<TypedMessage>(new M5(set_param));
  }

  virtual bool HandleReply(Reply* reply) override {
    return (reply->GetResponseCode() == 200);
  }

};

class M6Handler final : public MessageReceiver<TypedMessage::M6> {
 public:
  M6Handler(const InitParams& init_params)
    : MessageReceiver<TypedMessage::M6>(init_params) {
  }

  virtual bool HandleMessage(std::unique_ptr<TypedMessage> message) override {
    auto reply = std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
    reply->header().set_cseq(message->cseq());
    // todo: generate unique session id
    reply->header().set_session("abcdefg123456");
    sender_->SendRTSPData(reply->to_string());
    return true;
  }
};

M7Handler::M7Handler(const InitParams& init_params)
  : MessageReceiver<TypedMessage::M7>(init_params) {
}

bool M7Handler::HandleMessage(std::unique_ptr<TypedMessage> message) {
  if (!manager_->IsPaused()) {
    assert(observer_);
    observer_->OnError(this);
    return false;
  }

  auto reply = std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
  reply->header().set_cseq(message->cseq());
  manager_->Play();
  sender_->SendRTSPData(reply->to_string());
  return true;
}

M8Handler::M8Handler(const InitParams& init_params)
  : MessageReceiver<TypedMessage::M8>(init_params) {
}

bool M8Handler::HandleMessage(std::unique_ptr<TypedMessage> message) {
  auto reply = std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
  reply->header().set_cseq(message->cseq());
  manager_->Teardown(); // FIXME : make proper reset.
  sender_->SendRTSPData(reply->to_string());
  return true;
}

WfdSessionState::WfdSessionState(const InitParams& init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(new M5Handler(init_params));
  AddSequencedHandler(new M6Handler(init_params));
  AddSequencedHandler(new M7Handler(init_params));

  AddOptionalHandler(new M8Handler(init_params));
}

WfdSessionState::~WfdSessionState() {
}

}  // miracast
