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

#include "wfd/public/media_manager.h"

#include "cap_negotiation_state.h"
#include "streaming_state.h"
#include "wfd/parser/play.h"
#include "wfd/parser/reply.h"
#include "wfd/parser/setup.h"
#include "wfd/common/typed_message.h"
#include "wfd/parser/transportheader.h"

namespace wfd {

class M6Handler final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto setup = std::make_shared<WFD::Setup>(manager_->PresentationUrl());
    auto transport = new WFD::TransportHeader();
    transport->set_client_port(manager_->RtpPort());

    setup->header().set_transport(transport);
    setup->header().set_cseq(send_cseq_++);
    setup->header().set_require_wfd_support(true);

    return std::unique_ptr<M6>(new M6(setup));
  }

  virtual bool HandleReply(Reply* reply) override {
    std::string session_id = reply->message()->header().session();
    if(reply->GetResponseCode() == 200 && !session_id.empty()) {
      manager_->SetSession(session_id);
      return true;
    }

    return false;
  }
};

class M7Handler final : public SequencedMessageSender {
 public:
    using SequencedMessageSender::SequencedMessageSender;
 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override {
    auto play = std::make_shared<WFD::Play>(manager_->PresentationUrl());
    play->header().set_session(manager_->Session());
    play->header().set_cseq(send_cseq_++);
    play->header().set_require_wfd_support(true);

    return std::unique_ptr<M7>(new M7(play));
  }

  virtual bool HandleReply(Reply* reply) override {
    return (reply->GetResponseCode() == 200);
  }
};

WfdSessionState::WfdSessionState(const InitParams& init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(new M6Handler(init_params));
  AddSequencedHandler(new M7Handler(init_params));

  AddOptionalHandler(new M3Handler(init_params));
  AddOptionalHandler(new M4Handler(init_params));
  AddOptionalHandler(new TeardownHandler(init_params));
}

WfdSessionState::~WfdSessionState() {
}

}  // miracast
