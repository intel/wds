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

#ifndef SOURCE_H_
#define SOURCE_H_

#include "peer.h"

namespace wfd {

class MediaManager;

class Source : public Peer {
 public:
  virtual ~Source() {}
  static Source* Create(Peer::Delegate* delegate, MediaManager* mng);
  // these send M5 wfd_trigger_method messages
  // return 'false' if M5 cannot be send at the moment.
  virtual bool Teardown() = 0;
  virtual bool Play() = 0;
  virtual bool Pause() = 0;
};

}

#endif // SOURCE_H_
