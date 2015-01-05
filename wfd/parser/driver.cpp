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

namespace wfd {

Driver::~Driver() {
}

void Driver::Parse(const std::string& input, std::unique_ptr<Message>& message) {
  std::istringstream in(input);
  if (!in.good())
    return;

  scanner_.reset(new Scanner(&in, message));
  parser_.reset(new Parser(*scanner_, message));

  // todo: remove, just for testing
  //scanner_->set_debug(1);
  //parser_->set_debug_level(1);

  parser_->parse();
}

} // namespace wfd

