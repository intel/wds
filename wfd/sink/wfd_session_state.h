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

#include "wfd/common/message_handler.h"

namespace wfd {
namespace sink {

class M6Handler final : public SequencedMessageSender {
 public:
  M6Handler(const InitParams& init_params, uint& keep_alive_timer);

 private:
  virtual std::unique_ptr<Message> CreateMessage() override;
  virtual bool HandleReply(Reply* reply) override;

  uint& keep_alive_timer_;
};

class M16Handler final : public MessageReceiver<Request::M16> {
 public:
  M16Handler(const InitParams& init_params, uint& keep_alive_timer);

 private:
  virtual bool HandleTimeoutEvent(uint timer_id) const override;
  virtual std::unique_ptr<Reply> HandleMessage(Message* message) override;

  uint& keep_alive_timer_;
};

// WFD session state for RTSP sink.
// Includes M6, M7, M8 messages handling and optionally can handle M3, M4, M16
class WfdSessionState : public MessageSequenceWithOptionalSetHandler {
 public:
  WfdSessionState(const InitParams& init_params, MessageHandlerPtr m6_handler, MessageHandlerPtr m16_handler);
  virtual ~WfdSessionState();
};

}  // sink
}  // wfd

#endif // WFD_SESSION_STATE_H_
