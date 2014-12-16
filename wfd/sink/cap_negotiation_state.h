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

#ifndef CAP_NEGOTIATION_STATE_H_
#define CAP_NEGOTIATION_STATE_H_

#include "wfd/common/message_handler.h"

namespace wfd {
namespace sink {

// Capability negotiation state for RTSP sink.
// Includes M3 and M4 messages handling
class CapNegotiationState : public MessageSequenceWithOptionalSetHandler {
 public:
  CapNegotiationState(const InitParams& init_params);
  virtual ~CapNegotiationState();
};

class M4Handler final : public MessageReceiver<TypedMessage::M4> {
 public:
  M4Handler(const InitParams& init_params);
  virtual std::unique_ptr<WFD::Reply> HandleMessage(TypedMessage* message) override;
};

class M3Handler final : public MessageReceiver<TypedMessage::M3> {
 public:
  M3Handler(const InitParams& init_params);
  virtual std::unique_ptr<WFD::Reply> HandleMessage(TypedMessage* message) override;
};

}  // sink
}  // wfd

#endif // CAP_NEGOTIATION_STATE_H_
