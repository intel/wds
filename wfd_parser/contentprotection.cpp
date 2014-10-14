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


#include "contentprotection.h"

#include <assert.h>

namespace WFD {

namespace {
  const char* name[] = {"HDCP2.0", "HDCP2.1"};
  const char port_prefix[] = "port=";
}

ContentProtection::ContentProtection(HDCPSpec hdcp_spec, unsigned int port)
  : Property (WFD_CONTENT_PROTECTION),
    hdcp_spec_(hdcp_spec),
    port_(port) {
}

ContentProtection::ContentProtection(): Property (WFD_CONTENT_PROTECTION, true) {
}

ContentProtection::~ContentProtection() {
}

ContentProtection::HDCPSpec ContentProtection::hdcp_spec() const {
  assert(hdcp_spec_ != HDCP_SPEC_2_0
      || hdcp_spec_ != HDCP_SPEC_2_1);
  return hdcp_spec_;
}

std::string ContentProtection::to_string() const {
  std::string ret =
      PropertyName::wfd_content_protection
    + std::string(SEMICOLON) + std::string(SPACE);

  if (is_none())
    ret += WFD::NONE;
  else
    ret += name[hdcp_spec()] + std::string(SPACE)
        + port_prefix + std::to_string(port_);
  return ret;
}

}  // namespace WFD
