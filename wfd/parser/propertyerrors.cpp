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


#include "propertyerrors.h"

#include <assert.h>

namespace WFD {

PropertyErrors::PropertyErrors(PropertyType type, std::vector<unsigned short> error_codes) :
     type_(type),
     error_codes_(error_codes) {

}

PropertyErrors::PropertyErrors(const std::string& generic_property_name, std::vector<unsigned short> error_codes) :
     type_(WFD_GENERIC),
     generic_property_name_(generic_property_name),
     error_codes_(error_codes) {
         
}

PropertyErrors::~PropertyErrors() {
}

std::string PropertyErrors::to_string() const {
  std::string ret;

  if (type_ == WFD_GENERIC)
    ret += generic_property_name_;
  else
    ret += PropertyName::name[type_];

  ret += std::string(SEMICOLON) + std::string(SPACE);

  auto it = error_codes_.begin();
  while (it != error_codes_.end()) {
    ret += std::to_string(*it);
    it++;
    if (it != error_codes_.end())
      ret += ", ";
  }

  return ret;
}

}  // namespace WFD
