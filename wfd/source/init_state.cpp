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

#include "init_state.h"

#include <algorithm>

#include "media_manager.h"
#include "getparameter.h"
#include "options.h"
#include "reply.h"
#include "setparameter.h"
#include "typed_message.h"

namespace wfd {

class M1Handler final : public SequencedMessageSender {
 public:
//  M1Handler(Peer::Delegate* sender, MediaManager* manager, Observer* observer)
//    : SequencedMessageSender(manager, observer) {
//  }
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto options = std::make_shared<WFD::Options>("*");
    options->header().set_cseq(send_cseq_++);
    options->header().set_require_wfd_support(true);
    return std::unique_ptr<M1>(new M1(options));
  }

  virtual bool HandleReply(Reply* reply) override {
    return (reply->GetResponseCode() == 200);
  }

};

class M2Handler final : public MessageReceiver<TypedMessage::M2> {
 public:
  M2Handler(const InitParams& init_params)
    : MessageReceiver<TypedMessage::M2>(init_params) {
  }
  virtual bool HandleMessage(std::unique_ptr<TypedMessage> message) override {
    auto reply = std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
    std::vector<WFD::Method> supported_methods;
    supported_methods.push_back(WFD::ORG_WFA_WFD_1_0);
    supported_methods.push_back(WFD::GET_PARAMETER);
    supported_methods.push_back(WFD::SET_PARAMETER);
    supported_methods.push_back(WFD::PLAY);
    supported_methods.push_back(WFD::PAUSE);
    supported_methods.push_back(WFD::SETUP);
    supported_methods.push_back(WFD::TEARDOWN);
    reply->header().set_supported_methods(supported_methods);
    reply->header().set_cseq(message->message()->header().cseq());
    sender_->SendRTSPData(reply->to_string());
    return true;
  }
};

InitState::InitState(const InitParams& init_params)
  : MessageSequenceHandler(init_params) {
  AddSequencedHandler(new M1Handler(init_params));
  AddSequencedHandler(new M2Handler(init_params));
}

InitState::~InitState() {
}

}  // miracast
