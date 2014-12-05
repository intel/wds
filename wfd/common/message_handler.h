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

#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#include <cassert>
#include <queue>
#include <vector>
#include <memory>
#include <utility>

#include "peer.h"
#include "typed_message.h"

namespace wfd {

class MediaManager;

class MessageHandler {
 public:
  class Observer {
   public:
    virtual void OnCompleted(MessageHandler* handler) {}
    virtual void OnError(MessageHandler* handler) {}

   protected:
    virtual ~Observer() {}
  };

  struct InitParams {
    Peer::Delegate* sender;
    MediaManager* manager;
    Observer* observer;
  };

  virtual ~MessageHandler() {}

  virtual void Start() = 0;
  virtual void Reset() = 0;

  virtual bool CanSend(TypedMessage* message) const = 0;
  virtual void Send(std::unique_ptr<TypedMessage> message) = 0;

  virtual bool CanHandle(TypedMessage* message) const = 0;
  virtual void Handle(std::unique_ptr<TypedMessage> message) = 0;

  void set_observer(Observer* observer) {
    assert(observer);
    observer_ = observer;
  }

protected:
  explicit MessageHandler(const InitParams& init_params)
    : sender_(init_params.sender),
      manager_(init_params.manager),
      observer_(init_params.observer) {
    assert(sender_);
    assert(manager_);
    assert(observer_);
  }

  Peer::Delegate* sender_;
  MediaManager* manager_;
  Observer* observer_;
  // States should be handled within one thread. It's OK to have this static.
  static int send_cseq_;
};

class MessageSequenceHandler : public MessageHandler,
                               public MessageHandler::Observer {
 public:
  explicit MessageSequenceHandler(const InitParams& init_params);
  virtual ~MessageSequenceHandler();
  virtual void Start() override;
  virtual void Reset() override;

  virtual bool CanSend(TypedMessage* message) const override;
  virtual void Send(std::unique_ptr<TypedMessage> message) override;

  virtual bool CanHandle(TypedMessage* message) const override;
  virtual void Handle(std::unique_ptr<TypedMessage> message) override;

 protected:
  void AddSequencedHandler(MessageHandler* handler);
  // MessageHandler::Observer implementation.
  virtual void OnCompleted(MessageHandler* handler) override;
  virtual void OnError(MessageHandler* handler) override;

  std::vector<MessageHandler*> handlers_;
  MessageHandler* current_handler_;
};

class MessageSequenceWithOptionalSetHandler : public MessageSequenceHandler {
 public:
  explicit MessageSequenceWithOptionalSetHandler(const InitParams& init_params);
  virtual ~MessageSequenceWithOptionalSetHandler();
  virtual void Start() override;
  virtual void Reset() override;
  virtual bool CanSend(TypedMessage* message) const override;
  virtual void Send(std::unique_ptr<TypedMessage> message) override;
  virtual bool CanHandle(TypedMessage* message) const override;
  virtual void Handle(std::unique_ptr<TypedMessage> message) override;

 protected:
  void AddOptionalHandler(MessageHandler* handler);
  // MessageHandler::Observer implementation.
  virtual void OnCompleted(MessageHandler* handler) override;
  virtual void OnError(MessageHandler* handler) override;

  std::vector<MessageHandler*> optional_handlers_;
};

// This is aux classes to handle single message.
// There are two common scenarious:
// 1. We send a message and wait for reply
// class Handler : public MessageSender
//
// 2. We wait for the message and reply ourselves.
// class Handler : public MessageReceiver<type of the message
// we're waiting for>
template <TypedMessage::Type type>
class MessageReceiver : public MessageHandler {
 public:
  static_assert(type != TypedMessage::Reply,
                "MessageSender class should be used");

  explicit MessageReceiver(const InitParams& init_params)
    : MessageHandler(init_params),
      wait_for_message_(false) {
  }
  virtual ~MessageReceiver() {}

 protected:
  virtual bool HandleMessage(std::unique_ptr<TypedMessage> message) = 0;
  virtual bool CanHandle(TypedMessage* message) const override {
    assert(message);
    return wait_for_message_ && (type == message->type());
  }

 private:
  virtual void Start() override { wait_for_message_ = true; }
  virtual void Reset() override { wait_for_message_ = false; }
  virtual bool CanSend(TypedMessage* message) const override {
    return false;
  }
  virtual void Send(std::unique_ptr<TypedMessage> message) override {}
  virtual void Handle(std::unique_ptr<TypedMessage> message) override {
    assert(message);
    if (!CanHandle(message.get())) {
      observer_->OnError(this);
      return;
    }
    wait_for_message_ = false;
    if (HandleMessage(std::move(message)))
      observer_->OnCompleted(this);
    else
      observer_->OnError(this);
  }

  bool wait_for_message_;
};

class MessageSender : public MessageHandler {
 public:
  explicit MessageSender(const InitParams& init_params);
  virtual ~MessageSender();

 protected:
  virtual bool HandleReply(Reply* reply) = 0;
  virtual void Send(std::unique_ptr<TypedMessage> message) override;
  virtual void Reset() override;

 private:
  virtual bool CanHandle(TypedMessage* message) const override;
  virtual void Handle(std::unique_ptr<TypedMessage> message) override;

  std::queue<int> cseq_queue_;
};

// To be used for optional senders.
template <TypedMessage::Type type>
class TypedMessageSender : public MessageSender {
 public:
  using MessageSender::MessageSender;

  virtual ~TypedMessageSender() {}

 private:
  virtual void Start() override {}
  virtual bool CanSend(TypedMessage* message) const override {
    assert(message);
    return message->type() == type;
  }
};

// To be used for sequensed senders.
class SequencedMessageSender : public MessageSender {
 public:
  explicit SequencedMessageSender(const InitParams& init_params);
  virtual ~SequencedMessageSender();

 protected:
  virtual std::unique_ptr<TypedMessage> CreateMessage() = 0;

 private:
  virtual void Start() override;
  virtual void Reset() override;
  virtual bool CanSend(TypedMessage* message) const override;

  TypedMessage* to_be_send_;
};

}  // namespace wfd
#endif // MESSAGE_HANDLER_H_
