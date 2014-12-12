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

#include "typed_message.h"

#include <assert.h>

#include "wfd/parser/route.h"
#include "wfd/parser/connectortype.h"
#include "wfd/parser/standby.h"
#include "wfd/parser/idrrequest.h"
#include "wfd/parser/uibccapability.h"
#include "wfd/parser/uibcsetting.h"

namespace wfd {

TypedMessage::TypedMessage(WFD::MessagePtr message)
  : message_(message) {
}

TypedMessage::~TypedMessage() {
}

template <typename  T>
T* TypedMessage::get_property(WFD::PropertyType type) const {
  return reinterpret_cast<T*>(message()->payload().get_property(type).get());
}

int TypedMessage::cseq() const {
  return message_->header().cseq();
}

Reply::Reply(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::Reply>(message) {
}

int Reply::GetResponseCode() const {
  const WFD::Reply* reply = static_cast<const WFD::Reply*>(message());
  return reply->response_code();
}

M1::M1(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M1>(message) {
}

M2::M2(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M2>(message) {
}

M3::M3(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M3>(message) {
}

M4::M4(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M4>(message) {
}

M5::M5(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M5>(message) {
}

WFD::TriggerMethod::Method M5::trigger_method() const {
  return get_property<WFD::TriggerMethod>(WFD::WFD_TRIGGER_METHOD)->method();
}

M6::M6(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M6>(message) {
}

M7::M7(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M7>(message) {
}

M8::M8(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M8>(message) {
}

M9::M9(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M9>(message) {
}

M10::M10(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M10>(message) {
}

WFD::Route* M10::route() const {
  return get_property<WFD::Route>(WFD::WFD_ROUTE);
}

M11::M11(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M11>(message) {
}

WFD::ConnectorType* M11::connector_type() const {
  return get_property<WFD::ConnectorType>(WFD::WFD_CONNECTOR_TYPE);
}

M12::M12(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M12>(message) {
}

WFD::Standby* M12::standby() const {
  return get_property<WFD::Standby>(WFD::WFD_STANDBY);
}

M13::M13(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M13>(message) {
}

WFD::IDRRequest* M13::idr_request() const {
  return get_property<WFD::IDRRequest>(WFD::WFD_IDR_REQUEST);
}

M14::M14(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M14>(message) {
}

WFD::UIBCCapability* M14::uibc_capability() const {
  return get_property<WFD::UIBCCapability>(WFD::WFD_UIBC_CAPABILITY);
}

M15::M15(WFD::MessagePtr message)
  : TypedMessageBase<TypedMessage::M15>(message) {
}

WFD::UIBCSetting* M15::uibc_setting() const {
  return get_property<WFD::UIBCSetting>(WFD::WFD_UIBC_SETTING);
}

}
