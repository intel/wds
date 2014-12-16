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

#ifndef TYPED_MESSAGE_H_
#define TYPED_MESSAGE_H_

#include <memory>

#include "wfd/parser/message.h"
#include "wfd/parser/reply.h"
#include "wfd/parser/triggermethod.h"

namespace WFD {
  class ConnectorType;
  class IDRRequest;
  class Payload;
  class Route;
  class Standby;
  class UIBCCapability;
  class UIBCSetting;
}

namespace wfd {

class TypedMessage {
 public:
  enum Type {
    M1, M2, M3, M4, M5, M6, M7, M8, M9, M10, M11, M12, M13, M14,
    M15, M16, Reply
  };

  virtual ~TypedMessage();
  virtual TypedMessage::Type type() const = 0;

  const WFD::Message* message() const { return message_.get(); }
  const WFD::Payload& payload() const { return message_->payload(); }

  // Utility methods
  int cseq() const;

 protected:
  explicit TypedMessage(WFD::MessagePtr message);
  template <typename T>
  T* get_property(WFD::PropertyType) const;

  WFD::MessagePtr message_;
};

template <TypedMessage::Type typeArg>
class TypedMessageBase : public TypedMessage {
  virtual TypedMessage::Type type() const override final { return typeArg; }

 protected:
  explicit TypedMessageBase(WFD::MessagePtr message)
      : TypedMessage(message) {}
};

class Reply : public TypedMessageBase<TypedMessage::Reply> {
 public:
  explicit Reply(WFD::MessagePtr message);

  int GetResponseCode() const;
};

class M1 : public TypedMessageBase<TypedMessage::M1> {
 public:
  explicit M1(WFD::MessagePtr message);
};

class M2 : public TypedMessageBase<TypedMessage::M2> {
 public:
  explicit M2(WFD::MessagePtr message);
};

class M3 : public TypedMessageBase<TypedMessage::M3> {
 public:
  explicit M3(WFD::MessagePtr message);
};

class M4 : public TypedMessageBase<TypedMessage::M4> {
 public:
  explicit M4(WFD::MessagePtr message);
};

class M5 : public TypedMessageBase<TypedMessage::M5> {
 public:
  explicit M5(WFD::MessagePtr message);
  WFD::TriggerMethod::Method trigger_method() const;
};

class M6 : public TypedMessageBase<TypedMessage::M6> {
 public:
  explicit M6(WFD::MessagePtr message);
};

class M7 : public TypedMessageBase<TypedMessage::M7> {
 public:
  explicit M7(WFD::MessagePtr message);
};

class M8 : public TypedMessageBase<TypedMessage::M8> {
 public:
  explicit M8(WFD::MessagePtr message);
};

class M9 : public TypedMessageBase<TypedMessage::M9> {
 public:
  explicit M9(WFD::MessagePtr message);
};

class M10 : public TypedMessageBase<TypedMessage::M10> {
 public:
  explicit M10(WFD::MessagePtr message);
  WFD::Route* route() const;
};

class M11 : public TypedMessageBase<TypedMessage::M11> {
 public:
  explicit M11(WFD::MessagePtr message);
  WFD::ConnectorType* connector_type() const;
};

class M12 : public TypedMessageBase<TypedMessage::M12> {
 public:
  explicit M12(WFD::MessagePtr message);
  WFD::Standby* standby() const;
};

class M13 : public TypedMessageBase<TypedMessage::M13> {
 public:
  explicit M13(WFD::MessagePtr message);
  WFD::IDRRequest* idr_request() const;
};

class M14 : public TypedMessageBase<TypedMessage::M14> {
 public:
  explicit M14(WFD::MessagePtr message);
  WFD::UIBCCapability* uibc_capability() const;
};

class M15 : public TypedMessageBase<TypedMessage::M15> {
 public:
  explicit M15(WFD::MessagePtr message);
  WFD::UIBCSetting* uibc_setting() const;
};

}

#endif  /* TYPED_MESSAGE_H_ */
