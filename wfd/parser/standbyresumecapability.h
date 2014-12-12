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


#ifndef STANDBYRESUMECAPABILITY_H_
#define STANDBYRESUMECAPABILITY_H_

#include "property.h"

namespace WFD {

class StandbyResumeCapability: public Property {
public:
  explicit StandbyResumeCapability(bool is_supported);
  virtual ~StandbyResumeCapability();

  virtual std::string to_string() const override;
};

}  // namespace WFD

#endif  // STANDBYRESUMECAPABILITY_H_
