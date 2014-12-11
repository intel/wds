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
#include "wfd/common/typed_message.h"
#include "wfd_session_state.h"

namespace wfd {

namespace {

// todo: check mandatory parameters for each message
std::unique_ptr<TypedMessage> CreateTypedMessage(WFD::MessagePtr message) {
  switch(message->type()) {
  case WFD::Message::MessageTypeReply:
    return std::unique_ptr<TypedMessage>(new Reply(message));
  case WFD::Message::MessageTypeOptions:
    return std::unique_ptr<TypedMessage>(new M1(message));
  case WFD::Message::MessageTypeGetParameter:
    return std::unique_ptr<TypedMessage>(new M3(message));
  case WFD::Message::MessageTypeSetParameter:
    if (message->payload().has_property(WFD::WFD_PRESENTATION_URL))
      return std::unique_ptr<TypedMessage>(new M4(message));
    if (message->payload().has_property(WFD::WFD_TRIGGER_METHOD))
      return std::unique_ptr<TypedMessage>(new M5(message));
  default:
      break;
      // TODO: warning.
  }

  return nullptr;
}

}

class SinkStateMachine : public MessageSequenceHandler {
 public:
   SinkStateMachine(const InitParams& init_params)
     : MessageSequenceHandler(init_params) {
     AddSequencedHandler(new sink::InitState(init_params));
     AddSequencedHandler(new sink::CapNegotiationState(init_params));
     AddSequencedHandler(new sink::WfdSessionState(init_params));
     AddSequencedHandler(new sink::StreamingState(init_params));
   }
   SinkStateMachine(Peer::Delegate* sender, MediaManager* mng)
     : SinkStateMachine({sender, mng, this}) {}

   int GetNextCSeq() { return send_cseq_++; }
};

class SinkImpl final : public Sink, public RTSPInputHandler {
 public:
  SinkImpl(Delegate* delegate, MediaManager* mng);

 private:
  // Sink implementation.
  virtual void Start() override;
  virtual void RTSPDataReceived(const std::string& message) override;
  virtual bool Teardown() override;
  virtual bool Play() override;
  virtual bool Pause() override;

  // RTSPInputHandler
  virtual void MessageParsed(WFD::MessagePtr message) override;

  bool HandleCommand(std::unique_ptr<TypedMessage> command);

  template <typename WfdMessage, typename TypedWfdMessage>
  std::unique_ptr<TypedMessage> CreateCommand();

  std::unique_ptr<SinkStateMachine> state_machine_;
  MediaManager* manager_;
};

SinkImpl::SinkImpl(Delegate* delegate, MediaManager* mng)
  : state_machine_(new SinkStateMachine(delegate, mng)),
    manager_(mng) {
}

void SinkImpl::Start() {
  state_machine_->Start();
}

void SinkImpl::RTSPDataReceived(const std::string& message) {
  InputReceived(message);
}

template <typename WfdMessage, typename TypedWfdMessage>
std::unique_ptr<TypedMessage> SinkImpl::CreateCommand() {
  auto message = std::make_shared<WfdMessage>(manager_->PresentationUrl());
  message->header().set_session(manager_->Session());
  message->header().set_cseq(state_machine_->GetNextCSeq());
  return std::unique_ptr<TypedMessage>(new TypedWfdMessage(message));
}

bool SinkImpl::HandleCommand(std::unique_ptr<TypedMessage> command) {
  if (manager_->Session().empty() ||
      manager_->PresentationUrl().empty())
    return false;

  if (!state_machine_->CanSend(command.get()))
    return false;
  state_machine_->Send(std::move(command));
  return true;
}

bool SinkImpl::Teardown() {
  return HandleCommand(CreateCommand<WFD::Teardown, wfd::M8>());
}

bool SinkImpl::Play() {
  return HandleCommand(CreateCommand<WFD::Play, wfd::M7>());
}

bool SinkImpl::Pause() {
  return HandleCommand(CreateCommand<WFD::Pause, wfd::M9>());
}

void SinkImpl::MessageParsed(WFD::MessagePtr message) {
  auto typed_message = CreateTypedMessage(message);
  if (typed_message)
    state_machine_->Handle(std::move(typed_message));
}

WFD_EXPORT Sink* Sink::Create(Delegate* delegate, MediaManager* mng) {
  return new SinkImpl(delegate, mng);
}

}  // namespace wfd
