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


#ifndef DRIVER_H_
#define DRIVER_H_

#include <string>
#include <memory>

#include "scanner.h"
#include "message.h"
#include "payload.h"
#include "parser.tab.hpp"

namespace WFD {


class Driver {
 public:
  Driver();
  virtual ~Driver();

  void parse_header(const std::string& message);
  void parse_payload(const std::string& message);
  std::shared_ptr<Message> parsed_message() const { return message_; }

 private:
  friend class Parser;
  void set_message(Message* message);
  void set_payload(Payload* payload);
  void parse(const std::string& message);

 private:
  std::unique_ptr<Parser> parser_;
  std::unique_ptr<Scanner> scanner_;
  std::shared_ptr<Message> message_;
};

}  // namespace WFD

#endif  // DRIVER_H_
