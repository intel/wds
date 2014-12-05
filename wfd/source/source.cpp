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

#include "wfd/public/source.h"

#include <algorithm>

#include "cap_negotiation_state.h"
#include "init_state.h"
#include "streaming_state.h"
#include "wfd_session_state.h"
#include "wfd/common/message_handler.h"
#include "wfd/common/rtsp_input_handler.h"
#include "wfd/common/typed_message.h"
#include "wfd/parser/setparameter.h"

namespace wfd {

namespace {

std::unique_ptr<TypedMessage> CreateTypedMessage(WFD::MessagePtr message) {
  switch(message->type()) {
  case WFD::Message::MessageTypeReply:
    return std::unique_ptr<TypedMessage>(new Reply(message));
  case WFD::Message::MessageTypeOptions:
    return std::unique_ptr<TypedMessage>(new M2(message));
  case WFD::Message::MessageTypeSetup:
    return std::unique_ptr<TypedMessage>(new M6(message));
  case WFD::Message::MessageTypePlay:
    return std::unique_ptr<TypedMessage>(new M7(message));
  case WFD::Message::MessageTypeTeardown:
    return std::unique_ptr<TypedMessage>(new M8(message));
  case WFD::Message::MessageTypePause:
    return std::unique_ptr<TypedMessage>(new M9(message));
  case WFD::Message::MessageTypeSetParameter:
    if (message->payload().has_property(WFD::WFD_ROUTE))
      return std::unique_ptr<TypedMessage>(new M10(message));
    else if (message->payload().has_property(WFD::WFD_CONNECTOR_TYPE))
      return std::unique_ptr<TypedMessage>(new M11(message));
    else if (message->payload().has_property(WFD::WFD_STANDBY))
      return std::unique_ptr<TypedMessage>(new M12(message));
    else if (message->payload().has_property(WFD::WFD_IDR_REQUEST))
      return std::unique_ptr<TypedMessage>(new M13(message));
    else if (message->payload().has_property(WFD::WFD_UIBC_CAPABILITY))
      return std::unique_ptr<TypedMessage>(new M14(message));
    else if (message->payload().has_property(WFD::WFD_UIBC_SETTING))
      return std::unique_ptr<TypedMessage>(new M15(message));
  default:
      break;
      // TODO: warning.
  }

  return nullptr;
}

}

class SourceStateMachine : public MessageSequenceHandler {
 public:
   SourceStateMachine(const InitParams& init_params)
     : MessageSequenceHandler(init_params) {
     AddSequencedHandler(new InitState(init_params));
     AddSequencedHandler(new CapNegotiationState(init_params));
     AddSequencedHandler(new WfdSessionState(init_params));
     AddSequencedHandler(new StreamingState(init_params));
   }

   int GetNextCSeq() { return send_cseq_++; }
};

class SourceImpl final : public Source, public RTSPInputHandler, public MessageHandler::Observer {
 public:
  SourceImpl(Delegate* delegate, MediaManager* mng);

 private:
  // Source implementation.
  virtual void Start() override;
  virtual void RTSPDataReceived(const std::string& message) override;
  virtual bool Teardown() override;
  virtual bool Play() override;
  virtual bool Pause() override;

  // public MessageHandler::Observer
  virtual void OnCompleted(MessageHandler* handler) override;
  virtual void OnError(MessageHandler* handler) override;

  // RTSPInputHandler
  virtual void MessageParsed(WFD::MessagePtr message) override;

  std::unique_ptr<SourceStateMachine> state_machine_;
};

SourceImpl::SourceImpl(Delegate* delegate, MediaManager* mng)
  : state_machine_(new SourceStateMachine({delegate, mng, this})) {
}

void SourceImpl::Start() {
  state_machine_->Start();
}

void SourceImpl::RTSPDataReceived(const std::string& message) {
  InputReceived(message);
}

namespace  {

std::unique_ptr<TypedMessage> CreateM5(int send_cseq, WFD::TriggerMethod::Method method) {
  auto set_param =
      std::make_shared<WFD::SetParameter>("rtsp://localhost/wfd1.0");
  set_param->header().set_cseq(send_cseq);
  set_param->payload().add_property(
      std::shared_ptr<WFD::Property>(new WFD::TriggerMethod(method)));
  return std::unique_ptr<TypedMessage>(new M5(set_param));
}

}

bool SourceImpl::Teardown() {
  auto m5 = CreateM5(state_machine_->GetNextCSeq(),
                     WFD::TriggerMethod::TEARDOWN);

  if (!state_machine_->CanSend(m5.get()))
    return false;
  state_machine_->Send(std::move(m5));
  return true;
}

bool SourceImpl::Play() {
  auto m5 = CreateM5(state_machine_->GetNextCSeq(),
                     WFD::TriggerMethod::PLAY);

  if (!state_machine_->CanSend(m5.get()))
    return false;
  state_machine_->Send(std::move(m5));
  return true;
}

bool SourceImpl::Pause() {
  auto m5 = CreateM5(state_machine_->GetNextCSeq(),
                     WFD::TriggerMethod::PAUSE);

  if (!state_machine_->CanSend(m5.get()))
    return false;
  state_machine_->Send(std::move(m5));
  return true;
}

void SourceImpl::OnCompleted(MessageHandler* handler) {}

void SourceImpl::OnError(MessageHandler* handler) {}

void SourceImpl::MessageParsed(WFD::MessagePtr message) {
  auto typed_message = CreateTypedMessage(message);
  if (typed_message)
    state_machine_->Handle(std::move(typed_message));
}

Source* Source::Create(Delegate* delegate, MediaManager* mng) {
  return new SourceImpl(delegate, mng);
}

}  // namespace wfd
