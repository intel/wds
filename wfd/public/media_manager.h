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

#ifndef MEDIA_MANAGER_H_
#define MEDIA_MANAGER_H_

#include <string>

namespace wfd {

class MediaManager {
 public:  
  virtual ~MediaManager() {}
  virtual void Play() = 0;
  virtual void Pause() = 0;
  virtual void Teardown() = 0;
  virtual bool IsPaused() const = 0;
  virtual void SetRtpPorts(int port1, int port2) = 0;
};

}  // namespace wfd

#endif // MEDIA_MANAGER_H_

