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
#include "wfd_session_state.h"
#include "wfd/parser/reply.h"
#include "wfd/common/typed_message.h"

namespace wfd {
namespace source {

class M9Handler final : public MessageReceiver<TypedMessage::M9> {
 public:
  M9Handler(const InitParams& init_params)
    : MessageReceiver<TypedMessage::M9>(init_params) {
  }

  virtual std::unique_ptr<WFD::Reply> HandleMessage(
      TypedMessage* message) override {
    int response_code = 406;
    if (!manager_->IsPaused()) {
      manager_->Pause();
      response_code = 200;
    }
    return std::unique_ptr<WFD::Reply>(new WFD::Reply(response_code));
  }
};

class M5Sender final : public OptionalMessageSender<TypedMessage::M5> {
 public:
  M5Sender(const InitParams& init_params)
    : OptionalMessageSender<TypedMessage::M5>(init_params) {
  }
  virtual bool HandleReply(Reply* reply) override {
    return (reply->GetResponseCode() == 200);
  }
};

StreamingState::StreamingState(const InitParams& init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(new M8Handler(init_params));

  AddOptionalHandler(new M5Sender(init_params));
  AddOptionalHandler(new M7Handler(init_params));
  AddOptionalHandler(new M9Handler(init_params));
}

StreamingState::~StreamingState() {
}

}  // source
}  // wfd
