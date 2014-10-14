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


#include "message.h"

namespace WFD {

Message::Message(MessageType type, const std::string& request_uri)
  : type_(type),
    request_uri_(request_uri) {
}

Message::~Message() {
}

Message::MessageType Message::type() const {
  return type_;
}

void Message::set_request_uri(const std::string& request_uri) {
  request_uri_ = request_uri;
}

void Message::set_header(Header* header) {
  header_.reset(header);
}

Header& Message::header() const {
  if (!header_)
    header_.reset(new Header());
  return *header_;
}

void Message::set_payload(Payload* payload) {
  payload_.reset(payload);
}

Payload& Message::payload() const {
  if (!payload_)
    payload_.reset(new Payload());
  return *payload_;
}

std::string Message::to_string() {
  std::string ret;
  if (payload_)
    ret = payload_->to_string();


  if (header_) {
    header_->set_content_length (ret.length());
    ret = header_->to_string() + ret;
  }

  return ret;
}

} // namespace WFD
