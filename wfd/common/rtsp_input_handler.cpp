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

#include "rtsp_input_handler.h"

namespace wfd {

// FIXME : re-write in more robust way (handle granular input).
void RTSPInputHandler::InputReceived(const std::string& input) {
  rtsp_recieve_buffer_ += input;
  std::string buffer;

  while(GetHeader(buffer)) {
    Message* message = nullptr;
    driver_.Parse(buffer, message);
    if (!message) {
      // TODO : handle an invalid input.
      rtsp_recieve_buffer_.clear();
      return;
    }
    uint content_length = message->header().content_length();
    if (content_length && GetPayload(buffer, content_length))
      driver_.Parse(buffer, message);
    MessageParsed(std::unique_ptr<Message>(message));
  }
}

bool RTSPInputHandler::GetHeader(std::string& header) {
  size_t eom = rtsp_recieve_buffer_.find("\r\n\r\n");
  if (eom == std::string::npos) {
    rtsp_recieve_buffer_.clear();
    return false;
  }

  header = rtsp_recieve_buffer_.substr(0, eom + 4);
  rtsp_recieve_buffer_.erase(0, eom + 4);
  return true;
}

bool RTSPInputHandler::GetPayload(std::string& payload, unsigned content_length) {
  if (rtsp_recieve_buffer_.size() < content_length)
      return false;

  payload = rtsp_recieve_buffer_.substr(0, content_length);
  rtsp_recieve_buffer_.erase(0, content_length);
  return true;
}

}  // namespace wfd
