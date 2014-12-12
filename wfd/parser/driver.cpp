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


#include "driver.h"
#include "message.h"
#include "reply.h"

#include <cctype>
#include <sstream>

namespace WFD {

Driver::Driver() {
}

void Driver::parse_header(const std::string& message) {
  message_.reset();
  parse(message);
}

void Driver::parse_payload(const std::string& message) {
  if (!message_)
    return;
  parse(message);
}

void Driver::parse(const std::string& message) {
  std::istringstream in(message);
  if (!in.good())
    return;

  scanner_.reset(new Scanner(&in, *this));
  parser_.reset(new Parser(*scanner_, *this));

  // todo: remove, just for testing
  //scanner_->set_debug(1);
  //parser_->set_debug_level(1);

  parser_->parse();
}

void Driver::set_message(Message* message) {
  message_.reset(message);
}

void Driver::set_payload(Payload* payload) {
  if (message_)
    message_->set_payload(payload);
}

Driver::~Driver(){
}

} // namespace WFD

