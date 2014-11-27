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

#include "message_handler.h"

namespace wfd {

// Capability negotiation state for RTSP source.
// Includes M3 and M4 messages handling
class CapNegotiationState : public MessageSequenceHandler {
 public:
  CapNegotiationState(const InitParams& init_params);
  virtual ~CapNegotiationState();
};

}  // miracast

#endif // CAP_NEGOTIATION_STATE_H_
