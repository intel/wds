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


#ifndef CONNECTORTYPE_H_
#define CONNECTORTYPE_H_

#include "property.h"

namespace wfd {

class ConnectorType: public Property {
 public:
  ConnectorType();
  explicit ConnectorType(unsigned short connector_type);
  ~ConnectorType() override;

  unsigned short connector_type() const { return connector_type_; }
  std::string ToString() const override;

 private:
  unsigned short connector_type_;
};

}  // namespace wfd

#endif  // CONNECTORTYPE_H_
