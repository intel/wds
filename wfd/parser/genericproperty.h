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


#ifndef GENERIC_PROPERTY_H_
#define GENERIC_PROPERTY_H_

#include "property.h"

namespace WFD {

class GenericProperty: public Property {
  public:
    GenericProperty();
    GenericProperty(const std::string& key, const std::string& value);

    virtual ~GenericProperty();

    const std::string& key () const { return key_; }
    const std::string& value () const { return value_; }

    virtual std::string to_string() const override;
  private:
    std::string key_;
    std::string value_;
};

}  // namespace WFD

#endif  // GENERIC_PROPERTY_H_
