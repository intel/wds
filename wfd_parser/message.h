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


#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <memory>

#include "header.h"
#include "payload.h"

namespace WFD {

class Message {

 public:

  enum MessageType {
    MessageTypeOptions = 1,
    MessageTypeSetParameter,
    MessageTypeGetParameter,
    MessageTypeSetup,
    MessageTypePlay,
    MessageTypeTeardown,
    MessageTypePause,
    MessageTypeReply
  };

 public:
  Message(MessageType type, const std::string& request_uri = std::string());
  virtual ~Message();

  MessageType type() const;

  bool is_reply() const { return type() == MessageTypeReply; }
  bool is_request() const { return !is_reply(); }

  const std::string& request_uri() const { return request_uri_; }
  void set_request_uri(const std::string& request_uri);

  void set_header(Header* header);
  Header& header() const;

  void set_payload(Payload* payload);
  Payload& payload() const;

  virtual std::string to_string();

 protected:
  MessageType type_;

 private:
  std::string request_uri_;
  mutable std::unique_ptr<Header> header_;
  mutable std::unique_ptr<Payload> payload_;
};

}  // namespace WFD

#endif  // MESSAGE_H_
