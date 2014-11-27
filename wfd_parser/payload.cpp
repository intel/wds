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


#include "payload.h"

namespace WFD {

Payload::Payload() {
}

Payload::Payload(const std::vector<std::string>& properties)
  : request_properties_(properties) {

}

Payload::Payload(const PropertyMap& properties)
  : properties_(properties) {
}

Payload::Payload(const PropertyErrorMap& property_errors)
  : property_errors_(property_errors) {
}

Payload::~Payload() {
}

std::shared_ptr<WFD::Property> Payload::get_property(std::string name) const
{
  auto property = properties_.find(name);
  if (property != properties_.end())
    return (*property).second;
  return nullptr;
}

std::shared_ptr<WFD::Property> Payload::get_property(WFD::PropertyType type) const
{
  if (type == WFD_GENERIC)
    return nullptr;

  return get_property(WFD::PropertyName::name[type]);
}

bool Payload::has_property(WFD::PropertyType type) const {
  return properties_.find(WFD::PropertyName::name[type]) != properties_.end();
}

void Payload::add_property(const std::shared_ptr<Property>& property) {
  if (property->type() == WFD_GENERIC) {
    auto gen_prop = std::static_pointer_cast<GenericProperty>(property);
    properties_[gen_prop->key()] = property;
  } else {
    properties_[PropertyName::name[property->type()]] = property;
  }
}

const PropertyMap& Payload::properties() const {
  return properties_;
}

std::shared_ptr<WFD::PropertyErrors> Payload::get_property_error(std::string name) const
{
  auto property_error = property_errors_.find(name);
  if (property_error != property_errors_.end())
    return (*property_error).second;
  return nullptr;
}

std::shared_ptr<WFD::PropertyErrors> Payload::get_property_error(WFD::PropertyType type) const
{
  if (type == WFD_GENERIC)
    return nullptr;

  return get_property_error(WFD::PropertyName::name[type]);
}

void Payload::add_property_error(const std::shared_ptr<WFD::PropertyErrors>& errors) {
  if (errors->type() == WFD_GENERIC) {
    property_errors_[errors->generic_property_name()] = errors;
  } else {
    property_errors_[PropertyName::name[errors->type()]] = errors;
  }
}

const PropertyErrorMap& Payload::property_errors() const {
  return property_errors_;
}

void Payload::add_get_parameter_property(const PropertyType& type) {
  request_properties_.push_back(WFD::PropertyName::name[type]);
}

void Payload::add_get_parameter_property(const std::string& generic_property) {
  request_properties_.push_back(generic_property);
}

const std::vector<std::string> Payload::get_parameter_properties() const {
  return request_properties_;
}

std::string Payload::to_string() const {
  std::string ret;
  auto i = properties_.begin();
  auto end = properties_.end();

  while(i != end) {
    if ((*i).second) {
      ret += (*i).second->to_string();
      ret += "\r\n";
    }
    ++i;
  }

  auto req_i = request_properties_.begin();
  auto req_end = request_properties_.end();
  while(req_i != req_end) {
    ret += *req_i;
    ret += "\r\n";
    ++req_i;
  }

  auto error_i = property_errors_.rbegin();
  while(error_i != property_errors_.rend()) {
    ret += error_i->second->to_string();
    ret += "\r\n";
    error_i++;
  }

  return ret;
}

} // namespace WFD
