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

#ifndef PEER_H_
#define PEER_H_

#include <string>

namespace wfd {

class MessageSenderBase;

class Peer {
 public:

  class Delegate {
   public:
    virtual void SendRTSPData(const std::string& data) = 0;

    // Interfaces to manage timer events
    virtual uint CreateTimer(int seconds) = 0;
    virtual void ReleaseTimer(uint timer_id) = 0;

   protected:
    virtual ~Delegate() {}
  };

  virtual ~Peer() {}
  virtual void Start() = 0;
  virtual void RTSPDataReceived(const std::string& data) = 0;
  // These send M5 wfd_trigger_method messages for Peers that implement
  // Source functionality or M7, M8 and M9 for Sink implementations
  // return 'false' if message cannot be send at the moment.
  virtual bool Teardown() = 0;
  virtual bool Play() = 0;
  virtual bool Pause() = 0;

  virtual void OnTimerEvent(uint timer_id) = 0;
};

}

#endif // PEER_H_
