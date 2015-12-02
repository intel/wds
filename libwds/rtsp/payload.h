/*
 * This file is part of Wireless Display Software for Linux OS
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


#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "property.h"
#include "genericproperty.h"
#include "propertyerrors.h"

namespace wds {
namespace rtsp {

using PropertyMap = std::map<std::string, std::shared_ptr<Property>>;
using PropertyErrorMap = std::map<std::string, std::shared_ptr<PropertyErrors>>;

class Payload {
 public:
  Payload();
  explicit Payload(const std::vector<std::string>& request_properties);
  explicit Payload(const PropertyMap& properties);
  explicit Payload(const PropertyErrorMap& property_errors);
  virtual ~Payload();

  std::shared_ptr<Property> get_property(const std::string& name) const;
  std::shared_ptr<Property> get_property(PropertyType type) const;
  bool has_property(PropertyType type) const;
  void add_property(const std::shared_ptr<Property>& property);
  const PropertyMap& properties() const;

  void add_get_parameter_property(const PropertyType& property);
  void add_get_parameter_property(const std::string& generic_property);
  const std::vector<std::string>& get_parameter_properties() const;

  std::shared_ptr<PropertyErrors> get_property_error(const std::string& name) const;
  std::shared_ptr<PropertyErrors> get_property_error(PropertyType type) const;
  void add_property_error(const std::shared_ptr<PropertyErrors>& errors);
  const PropertyErrorMap& property_errors() const;

  std::string ToString() const;

 private:
  PropertyMap properties_;
  PropertyErrorMap property_errors_;
  std::vector<std::string> request_properties_;
};

}  // namespace rtsp
}  // namespace wds

#endif // PAYLOAD_H_
