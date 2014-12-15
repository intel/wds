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


#ifndef DISPLAYEDID_H_
#define DISPLAYEDID_H_

#include "property.h"

namespace wfd {

class DisplayEdid: public Property {
 public:
  DisplayEdid();
  DisplayEdid(unsigned short edid_block_count, std::string edid_payload);
  virtual ~DisplayEdid();

  unsigned short block_count() const { return edid_block_count_; }
  const std::string& payload() const { return edid_payload_; }
  virtual std::string to_string() const override;

 private:
  unsigned short edid_block_count_;
  std::string edid_payload_;
};

}  // namespace wfd

#endif  // DISPLAYEDID_H_
