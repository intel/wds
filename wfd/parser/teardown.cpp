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


#include "teardown.h"

namespace wfd {

Teardown::Teardown(const std::string& request_uri)
 : Request(Request::MethodTeardown, request_uri) {
}

Teardown::~Teardown() {
}

std::string Teardown::ToString() const {
  std::string ret = MethodName::TEARDOWN
      + std::string(SPACE) + request_uri()
      + std::string(SPACE) + std::string(RTSP_END) + std::string(CRLF);
  return ret + Message::ToString();
}

} /* namespace wfd */
