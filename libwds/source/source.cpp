/*
 * This file is part of Wireless Display Software for Linux OS
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

#include "libwds/public/source.h"


#include "cap_negotiation_state.h"
#include "init_state.h"
#include "streaming_state.h"
#include "session_state.h"
#include "libwds/common/message_handler.h"
#include "libwds/common/rtsp_input_handler.h"
#include "libwds/public/wds_export.h"
#include "libwds/parser/getparameter.h"
#include "libwds/parser/setparameter.h"
#include "libwds/public/media_manager.h"

namespace wds {

namespace {

bool InitializeRequestId(Request* request) {
  Request::ID id = Request::UNKNOWN;
  switch(request->method()) {
  case Request::MethodOptions:
    id = Request::M2;
    break;
  case Request::MethodSetup:
    id = Request::M6;
    break;
  case Request::MethodPlay:
    id = Request::M7;
    break;
  case Request::MethodTeardown:
    id = Request::M8;
    break;
  case Request::MethodPause:
    id = Request::M9;
    break;
  case Request::MethodSetParameter:
    if (request->payload().has_property(WFD_ROUTE))
      id = Request::M10;
    else if (request->payload().has_property(WFD_CONNECTOR_TYPE))
      id = Request::M11;
    else if (request->payload().has_property(WFD_STANDBY))
      id = Request::M12;
    else if (request->payload().has_property(WFD_IDR_REQUEST))
      id = Request::M13;
    else if (request->payload().has_property(WFD_UIBC_CAPABILITY))
      id = Request::M14;
    else if (request->payload().has_property(WFD_UIBC_SETTING))
      id = Request::M15;
    break;
  default:
    // TODO: warning.
    return false;
  }
  request->set_id(id);
  return true;
}

}

class SourceStateMachine : public MessageSequenceHandler {
 public:
   SourceStateMachine(const InitParams& init_params, uint& timer_id)
     : MessageSequenceHandler(init_params) {
     MessageHandlerPtr m16_sender = make_ptr(new source::M16Sender(init_params));
     AddSequencedHandler(make_ptr(new source::InitState(init_params)));
     AddSequencedHandler(make_ptr(new source::CapNegotiationState(init_params)));
     AddSequencedHandler(make_ptr(new source::SessionState(init_params, timer_id, m16_sender)));
     AddSequencedHandler(make_ptr(new source::StreamingState(init_params, m16_sender)));
   }

   int GetNextCSeq() { return send_cseq_++; }
};

class SourceImpl final : public Source, public RTSPInputHandler, public MessageHandler::Observer {
 public:
  SourceImpl(Delegate* delegate, SourceMediaManager* mng);

 private:
  // Source implementation.
  void Start() override;
  void Reset() override;
  void RTSPDataReceived(const std::string& message) override;
  bool Teardown() override;
  bool Play() override;
  bool Pause() override;

  // public MessageHandler::Observer
  void OnCompleted(MessageHandlerPtr handler) override;
  void OnError(MessageHandlerPtr handler) override;

  void OnTimerEvent(uint timer_id) override;

  // RTSPInputHandler
  void MessageParsed(std::unique_ptr<Message> message) override;

  // Keep-alive function
  void SendKeepAlive();
  void ResetAndTeardownMedia();

  std::shared_ptr<SourceStateMachine> state_machine_;
  uint keep_alive_timer_;
  Delegate* delegate_;
  SourceMediaManager* media_manager_;
};

SourceImpl::SourceImpl(Delegate* delegate, SourceMediaManager* mng)
  : state_machine_(new SourceStateMachine({delegate, mng, this}, keep_alive_timer_)),
    keep_alive_timer_(0),
    delegate_(delegate),
    media_manager_(mng) {
}

void SourceImpl::Start() {
  state_machine_->Start();
}

void SourceImpl::Reset() {
  state_machine_->Reset();
  delegate_->ReleaseTimer(keep_alive_timer_);
}

void SourceImpl::RTSPDataReceived(const std::string& message) {
  InputReceived(message);
}

void SourceImpl::OnTimerEvent(uint timer_id) {
  if (keep_alive_timer_ == timer_id)
    SendKeepAlive();
  else if (state_machine_->HandleTimeoutEvent(timer_id))
    ResetAndTeardownMedia();
}

void SourceImpl::SendKeepAlive() {
  delegate_->ReleaseTimer(keep_alive_timer_);
  auto get_param = std::unique_ptr<Request>(
      new GetParameter("rtsp://localhost/wfd1.0"));
  get_param->header().set_cseq(state_machine_->GetNextCSeq());
  get_param->set_id(Request::M16);

  if (state_machine_->CanSend(get_param.get())) {
    state_machine_->Send(std::move(get_param));
    keep_alive_timer_ =
        delegate_->CreateTimer(kDefaultKeepAliveTimeout - kDefaultTimeoutValue);
    assert(keep_alive_timer_);
  } else {
    ResetAndTeardownMedia();
  }
}

void SourceImpl::ResetAndTeardownMedia() {
  media_manager_->Teardown();
  state_machine_->Reset();
}

namespace  {

std::unique_ptr<Message> CreateM5(int send_cseq, TriggerMethod::Method method) {
  auto set_param = std::unique_ptr<Request>(
      new SetParameter("rtsp://localhost/wfd1.0"));
  set_param->header().set_cseq(send_cseq);
  set_param->payload().add_property(
      std::shared_ptr<Property>(new TriggerMethod(method)));
  set_param->set_id(Request::M5);
  return std::move(set_param);
}

}

bool SourceImpl::Teardown() {
  auto m5 = CreateM5(state_machine_->GetNextCSeq(),
                     TriggerMethod::TEARDOWN);

  if (!state_machine_->CanSend(m5.get()))
    return false;
  state_machine_->Send(std::move(m5));
  return true;
}

bool SourceImpl::Play() {
  auto m5 = CreateM5(state_machine_->GetNextCSeq(),
                     TriggerMethod::PLAY);

  if (!state_machine_->CanSend(m5.get()))
    return false;
  state_machine_->Send(std::move(m5));
  return true;
}

bool SourceImpl::Pause() {
  auto m5 = CreateM5(state_machine_->GetNextCSeq(),
                     TriggerMethod::PAUSE);

  if (!state_machine_->CanSend(m5.get()))
    return false;
  state_machine_->Send(std::move(m5));
  return true;
}

void SourceImpl::OnCompleted(MessageHandlerPtr handler) {
  assert(handler == state_machine_);
  ResetAndTeardownMedia();
}

void SourceImpl::OnError(MessageHandlerPtr handler) {
   assert(handler == state_machine_);
   ResetAndTeardownMedia();
}

void SourceImpl::MessageParsed(std::unique_ptr<Message> message) {
  if (message->is_request() && !InitializeRequestId(ToRequest(message.get()))) {
    WDS_ERROR("Cannot identify the received message");
    return;
  }
  if (!state_machine_->CanHandle(message.get())) {
    WDS_ERROR("Cannot handle the received message with Id: %d", ToRequest(message.get())->id());
    return;
  }
  state_machine_->Handle(std::move(message));
}

WDS_EXPORT Source* Source::Create(Delegate* delegate, SourceMediaManager* mng) {
  return new SourceImpl(delegate, mng);
}

}  // namespace wds
