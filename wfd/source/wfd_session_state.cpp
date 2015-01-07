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
#include "wfd/parser/reply.h"
#include "wfd/parser/setparameter.h"
#include "wfd/parser/triggermethod.h"

namespace wfd {
namespace source {

class M5Handler final : public SequencedMessageSender {
 public:
  using SequencedMessageSender::SequencedMessageSender;

 private:
  virtual std::unique_ptr<Message> CreateMessage() override {
    SetParameter* set_param = new SetParameter("rtsp://localhost/wfd1.0");
    set_param->header().set_cseq(send_cseq_++);
    set_param->payload().add_property(
        std::shared_ptr<wfd::Property>(new TriggerMethod(TriggerMethod::SETUP)));
    return std::unique_ptr<Message>(set_param);
  }

  virtual bool HandleReply(Reply* reply) override {
    return (reply->response_code() == 200);
  }

};

class M6Handler final : public MessageReceiver<Request::M6> {
 public:
  M6Handler(const InitParams& init_params)
    : MessageReceiver<Request::M6>(init_params) {
  }

  virtual std::unique_ptr<Reply> HandleMessage(
      Message* message) override {
    auto reply = std::unique_ptr<Reply>(new Reply(200));
    // todo: generate unique session id
    reply->header().set_session("abcdefg123456");

    auto transport = new TransportHeader();
    // we assume here that there is no coupled secondary sink
    transport->set_client_port(ToSourceMediaManager(manager_)->SinkRtpPorts().first);
    transport->set_server_port(ToSourceMediaManager(manager_)->SourceRtpPort());
    reply->header().set_transport(transport);

    return std::move(reply);
  }
};

M7Handler::M7Handler(const InitParams& init_params)
  : MessageReceiver<Request::M7>(init_params) {
}

std::unique_ptr<Reply> M7Handler::HandleMessage(
    Message* message) {
  if (!manager_->IsPaused())
    return nullptr; // FIXME : Shouldn't we just send error code?
  manager_->Play();
  return std::unique_ptr<Reply>(new Reply(200));
}

M8Handler::M8Handler(const InitParams& init_params)
  : MessageReceiver<Request::M8>(init_params) {
}

std::unique_ptr<Reply> M8Handler::HandleMessage(Message* message) {
  manager_->Teardown(); // FIXME : make proper reset.
  return std::unique_ptr<Reply>(new Reply(200));
}

WfdSessionState::WfdSessionState(const InitParams& init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(make_ptr(new M5Handler(init_params)));
  AddSequencedHandler(make_ptr(new M6Handler(init_params)));
  AddSequencedHandler(make_ptr(new M7Handler(init_params)));

  AddOptionalHandler(make_ptr(new M8Handler(init_params)));
}

WfdSessionState::~WfdSessionState() {
}

}  // source
}  // wfd
