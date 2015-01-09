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

#include "wfd/public/sink.h"

#include "cap_negotiation_state.h"
#include "init_state.h"
#include "wfd/common/message_handler.h"
#include "wfd/common/rtsp_input_handler.h"
#include "wfd/common/wfd_export.h"
#include "wfd/parser/pause.h"
#include "wfd/parser/play.h"
#include "wfd/parser/teardown.h"
#include "wfd/parser/triggermethod.h"
#include "wfd/public/media_manager.h"
#include "streaming_state.h"
#include "wfd_session_state.h"

namespace wfd {

namespace {

// todo: check mandatory parameters for each message
bool InitializeRequestId(Request* request) {
  Request::ID id;
  switch(request->method()) {
  case Request::MethodOptions:
    id = Request::M1;
    break;
  case Request::MethodGetParameter:
    if (request->payload().get_parameter_properties().empty())
      id = Request::M16;
    else
      id = Request::M3;
    break;
  case Request::MethodSetParameter:
    if (request->payload().has_property(WFD_PRESENTATION_URL))
      id = Request::M4;
    else if (request->payload().has_property(WFD_TRIGGER_METHOD))
      id = Request::M5;
    break;
  default:
    // TODO: warning.
    return false;
  }

  request->set_id(id);
  return true;
}

}

class SinkStateMachine : public MessageSequenceHandler {
 public:
   SinkStateMachine(const InitParams& init_params)
     : MessageSequenceHandler(init_params),
       keep_alive_timer_(0) {
     auto m6_handler = make_ptr(new sink::M6Handler(init_params, keep_alive_timer_));
     auto m16_handler = make_ptr(new sink::M16Handler(init_params, keep_alive_timer_));
     AddSequencedHandler(make_ptr(new sink::InitState(init_params)));
     AddSequencedHandler(make_ptr(new sink::CapNegotiationState(init_params)));
     AddSequencedHandler(make_ptr(new sink::WfdSessionState(init_params, m6_handler, m16_handler)));
     AddSequencedHandler(make_ptr(new sink::StreamingState(init_params, m16_handler)));
   }

   SinkStateMachine(Peer::Delegate* sender, SinkMediaManager* mng)
     : SinkStateMachine({sender, mng, this}) {}

   int GetNextCSeq() { return send_cseq_++; }

 private:
   uint keep_alive_timer_;
};

class SinkImpl final : public Sink, public RTSPInputHandler {
 public:
  SinkImpl(Delegate* delegate, SinkMediaManager* mng);

 private:
  // Sink implementation.
  virtual void Start() override;
  virtual void RTSPDataReceived(const std::string& message) override;
  virtual bool Teardown() override;
  virtual bool Play() override;
  virtual bool Pause() override;

  // RTSPInputHandler
  virtual void MessageParsed(std::unique_ptr<Message> message) override;

  virtual void OnTimerEvent(uint timer_id) override;

  bool HandleCommand(std::unique_ptr<Message> command);

  template <class WfdMessage, Request::ID id>
  std::unique_ptr<Message> CreateCommand();

  std::unique_ptr<SinkStateMachine> state_machine_;
  SinkMediaManager* manager_;
};

SinkImpl::SinkImpl(Delegate* delegate, SinkMediaManager* mng)
  : state_machine_(new SinkStateMachine(delegate, mng)),
    manager_(mng) {
}

void SinkImpl::Start() {
  state_machine_->Start();
}

void SinkImpl::RTSPDataReceived(const std::string& message) {
  InputReceived(message);
}

template <class WfdMessage, Request::ID id>
std::unique_ptr<Message> SinkImpl::CreateCommand() {
  auto message = new WfdMessage(manager_->PresentationUrl());
  message->header().set_session(manager_->Session());
  message->header().set_cseq(state_machine_->GetNextCSeq());
  message->set_id(id);
  return std::unique_ptr<Message>(message);
}

bool SinkImpl::HandleCommand(std::unique_ptr<Message> command) {
  if (manager_->Session().empty() ||
      manager_->PresentationUrl().empty())
    return false;

  if (!state_machine_->CanSend(command.get()))
    return false;
  state_machine_->Send(std::move(command));
  return true;
}

bool SinkImpl::Teardown() {
  return HandleCommand(CreateCommand<wfd::Teardown, Request::M8>());
}

bool SinkImpl::Play() {
  return HandleCommand(CreateCommand<wfd::Play, Request::M7>());
}

bool SinkImpl::Pause() {
  return HandleCommand(CreateCommand<wfd::Pause, Request::M9>());
}

void SinkImpl::MessageParsed(std::unique_ptr<Message> message) {
  if (message->is_request() && !InitializeRequestId(ToRequest(message.get())))
    return; // FIXME : Report error.
  state_machine_->Handle(std::move(message));
}

WFD_EXPORT Sink* Sink::Create(Delegate* delegate, SinkMediaManager* mng) {
  return new SinkImpl(delegate, mng);
}

void SinkImpl::OnTimerEvent(uint timer_id) {
  if (state_machine_->HandleTimeoutEvent(timer_id))
    state_machine_->Reset();
}

}  // namespace wfd
