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

#include "wfd/parser/message.h"
#include "wfd/parser/reply.h"
#include "wfd/public/peer.h"

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

  virtual bool CanSend(Message* message) const = 0;
  virtual void Send(std::unique_ptr<Message> message) = 0;

  virtual bool CanHandle(Message* message) const = 0;
  virtual void Handle(std::unique_ptr<Message> message) = 0;

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

  virtual bool CanSend(Message* message) const override;
  virtual void Send(std::unique_ptr<Message> message) override;

  virtual bool CanHandle(Message* message) const override;
  virtual void Handle(std::unique_ptr<Message> message) override;

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
  virtual bool CanSend(Message* message) const override;
  virtual void Send(std::unique_ptr<Message> message) override;
  virtual bool CanHandle(Message* message) const override;
  virtual void Handle(std::unique_ptr<Message> message) override;

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
class MessageReceiverBase : public MessageHandler {
 public:
  explicit MessageReceiverBase(const InitParams& init_params);
  virtual ~MessageReceiverBase();

 protected:
  virtual std::unique_ptr<wfd::Reply> HandleMessage(Message* message) = 0;
  virtual bool CanHandle(Message* message) const override;

 private:
  virtual void Start() override;
  virtual void Reset() override;
  virtual bool CanSend(Message* message) const override;
  virtual void Send(std::unique_ptr<Message> message) override;
  virtual void Handle(std::unique_ptr<Message> message) override;

  bool wait_for_message_;
};

template <Request::ID id>
class MessageReceiver : public MessageReceiverBase {
 public:
  using MessageReceiverBase::MessageReceiverBase;

 protected:
  virtual bool CanHandle(Message* message) const override {
    assert(message->is_request());
    return MessageReceiverBase::CanHandle(message) &&
           id == ToRequest(message)->id();
  }
};

class MessageSenderBase : public MessageHandler {
 public:
  explicit MessageSenderBase(const InitParams& init_params);
  virtual ~MessageSenderBase();

 protected:
  virtual bool HandleReply(Reply* reply) = 0;
  virtual void Send(std::unique_ptr<Message> message) override;
  virtual void Reset() override;

 private:
  virtual bool CanHandle(Message* message) const override;
  virtual void Handle(std::unique_ptr<Message> message) override;

  std::queue<int> cseq_queue_;
};

// To be used for optional senders.
template <Request::ID id>
class OptionalMessageSender : public MessageSenderBase {
 public:
  using MessageSenderBase::MessageSenderBase;

  virtual ~OptionalMessageSender() {}

 private:
  virtual void Start() override {}
  virtual bool CanSend(Message* message) const override {
    assert(message);
    assert(message->is_request());
    return ToRequest(message)->id() == id;
  }
};

// To be used for sequensed senders.
class SequencedMessageSender : public MessageSenderBase {
 public:
  explicit SequencedMessageSender(const InitParams& init_params);
  virtual ~SequencedMessageSender();

 protected:
  virtual std::unique_ptr<Message> CreateMessage() = 0;

 private:
  virtual void Start() override;
  virtual void Reset() override;
  virtual bool CanSend(Message* message) const override;

  Message* to_be_send_;
};

}  // namespace wfd
#endif // MESSAGE_HANDLER_H_
