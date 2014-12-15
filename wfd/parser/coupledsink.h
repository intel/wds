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


#ifndef COUPLEDSINK_H_
#define COUPLEDSINK_H_

#include "property.h"

namespace wfd {

class CoupledSink: public Property {
 public:
  CoupledSink();
  CoupledSink(unsigned char status, unsigned long long int sink_address);
  virtual ~CoupledSink();

  unsigned char status() const { return status_; }
  unsigned long long int sink_address() const { return sink_address_; }
  virtual std::string to_string() const override;

 private:
  unsigned short status_;
  unsigned long long int sink_address_;
};

}  // namespace wfd

#endif  // COUPLEDSINK_H_
