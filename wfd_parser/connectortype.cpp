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


#include "connectortype.h"

#include "macros.h"

namespace WFD {

ConnectorType::ConnectorType()
  : Property(WFD_CONNECTOR_TYPE, true) {
}

ConnectorType::ConnectorType(unsigned short connector_type)
  : Property(WFD_CONNECTOR_TYPE),
    connector_type_(connector_type) {
}

ConnectorType::~ConnectorType() {
}

std::string ConnectorType::to_string() const {
  std::string ret = PropertyName::wfd_connector_type + std::string(SEMICOLON)
    + std::string(SPACE);

  if (is_none()) {
    ret += WFD::NONE;
  } else {
    MAKE_HEX_STRING_2(connector_type, connector_type_);
    ret += connector_type;
  }

  return ret;
}

}  // namespace WFD
