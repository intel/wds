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


#ifndef I2C_H_
#define I2C_H_

#include "property.h"

namespace WFD {

class I2C: public Property {
 public:
  explicit I2C(int port);
  virtual ~I2C();

  bool is_supported() const { return port_ > 0; }
  int port() const { return port_; }
  virtual std::string to_string() const;

 private:
  int port_;
};

}  // namespace WFD

#endif  // I2C_H_
