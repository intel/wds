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


#include "displayedid.h"

#include "macros.h"

namespace wfd {

DisplayEdid::DisplayEdid(): Property(WFD_DISPLAY_EDID, true) {
}

DisplayEdid::DisplayEdid(unsigned short edid_block_count,
    std::string edid_payload)
  : Property(WFD_DISPLAY_EDID),
    edid_block_count_(edid_block_count),
    edid_payload_(edid_payload.length() ? edid_payload : wfd::NONE) {
}

DisplayEdid::~DisplayEdid() {
}

std::string DisplayEdid::to_string() const {

  std::string ret =
      PropertyName::wfd_display_edid + std::string(SEMICOLON)
    + std::string(SPACE);

  if (is_none()) {
    ret += wfd::NONE;
  } else {
    MAKE_HEX_STRING_2(edid_block_count, edid_block_count_);
    ret += edid_block_count + std::string(SPACE) + edid_payload_;
  }

  return ret;
}

}  // namespace wfd
