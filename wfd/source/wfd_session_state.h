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

#ifndef WFD_SESSION_STATE_H_
#define WFD_SESSION_STATE_H_

#include "message_handler.h"

namespace wfd {

// WFD session state for RTSP source.
// Includes M5, M6, M7 messages handling and optionally can handle M3, M4, M8
class WfdSessionState : public MessageSequenceWithOptionalSetHandler {
 public:
  WfdSessionState(const InitParams& init_params);
  virtual ~WfdSessionState();
};

class M8Handler final : public MessageReceiver<TypedMessage::M8> {
 public:
  M8Handler(const InitParams& init_params);

 private:
  virtual bool HandleMessage(std::unique_ptr<TypedMessage> message) override;
};

class M7Handler final : public MessageReceiver<TypedMessage::M7> {
 public:
  M7Handler(const InitParams& init_params);

 private:
  virtual bool HandleMessage(std::unique_ptr<TypedMessage> message) override;
};

}  // miracast

#endif // WFD_SESSION_STATE_H_
