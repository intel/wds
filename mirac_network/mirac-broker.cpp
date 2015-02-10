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

#include <glib-unix.h>
#include <algorithm>

#include "mirac-broker.hpp"
#include "mirac-glib-logging.hpp"

struct TimerCallbackData {
  TimerCallbackData(MiracBroker* delegate)
    : delegate_(delegate), timer_id_(0) {}
  MiracBroker* delegate_;
  uint timer_id_;
};

/* static C callback wrapper */
gboolean MiracBroker::send_cb (gint fd, GIOCondition condition, gpointer data_ptr)
{
    auto broker = static_cast<MiracBroker**> (data_ptr);
    return (*broker)->send_cb(fd, condition);
}

/* static C callback wrapper */
gboolean MiracBroker::receive_cb (gint fd, GIOCondition condition, gpointer data_ptr)
{
    auto broker = static_cast<MiracBroker**> (data_ptr);
    return (*broker)->receive_cb(fd, condition);
}

/* static C callback wrapper */
gboolean MiracBroker::listen_cb (gint fd, GIOCondition condition, gpointer data_ptr)
{
    auto broker = static_cast<MiracBroker**> (data_ptr);
    return (*broker)->listen_cb(fd, condition);
}

/* static C callback wrapper */
gboolean MiracBroker::connect_cb (gint fd, GIOCondition condition, gpointer data_ptr)
{
    auto broker = static_cast<MiracBroker**> (data_ptr);
    return (*broker)->connect_cb(fd, condition);
}

/* static C callback wrapper */
gboolean MiracBroker::try_connect (gpointer data_ptr)
{
    auto broker = static_cast<MiracBroker*> (data_ptr);
    broker->try_connect();
    return false;
}

gboolean MiracBroker::send_cb (gint fd, GIOCondition condition)
{
    try {
        if (!connection_->Send())
            return G_SOURCE_CONTINUE;
    } catch (const MiracConnectionLostException &exception) {
        on_connection_failure(CONNECTION_LOST);
    } catch (const std::exception &x) {
        WFD_WARNING("exception: %s", x.what());
    }
    return G_SOURCE_REMOVE;
}


gboolean MiracBroker::receive_cb (gint fd, GIOCondition condition)
{
    std::string msg;
    try {
        if (connection_->Receive(msg)) {
            WFD_VLOG("Received RTSP message:\n%s", msg.c_str());
            got_message (msg);
        }
    } catch (const MiracConnectionLostException &exception) {
        on_connection_failure(CONNECTION_LOST);
        return G_SOURCE_REMOVE;
    }
    return G_SOURCE_CONTINUE;
}

gboolean MiracBroker::listen_cb (gint fd, GIOCondition condition)
{
    try {
        connection(network_->Accept());
        WFD_LOG("connection from: %s", connection_->GetPeerAddress().c_str());
        on_connected();
    } catch (const std::exception &x) {
        WFD_WARNING("exception: %s", x.what());
    }

    return G_SOURCE_CONTINUE;
}

gboolean MiracBroker::connect_cb (gint fd, GIOCondition condition)
{
    try {
        if (!network_->Connect(NULL, NULL))
            return G_SOURCE_CONTINUE;
        WFD_LOG("connection success to: %s", network_->GetPeerAddress().c_str());
        connection(network_.release());

        /* make sure any network event sources are removed */
        network(NULL);

        on_connected();
    } catch (const std::exception &x) {
        gdouble elapsed = 1000 * g_timer_elapsed(connect_timer_, NULL);
        if (elapsed + connect_wait_ > connect_timeout_) {
            on_connection_failure(CONNECTION_TIMEOUT);
        } else {
            connect_wait_id_ = g_timeout_add (connect_wait_, try_connect, this);
        }
    }
    return G_SOURCE_REMOVE;
}

void MiracBroker::network(MiracNetwork *connection)
{
    while (g_source_remove_by_user_data(&network_source_ptr_))
        ;
    network_.reset(connection);
}

void MiracBroker::connection(MiracNetwork *connection)
{
    while (g_source_remove_by_user_data(&connection_source_ptr_))
        ;
    connection_.reset(connection);

    if (connection_)
        g_unix_fd_add(connection_->GetHandle(), G_IO_IN,
                      receive_cb, &connection_source_ptr_);
}

void MiracBroker::try_connect()
{
    WFD_LOG("Trying to connect...");

    connect_wait_id_ = 0;
    network(new MiracNetwork());

    if (network_->Connect(peer_address_.c_str(), peer_port_.c_str())) {
        g_unix_fd_add(network_->GetHandle(), G_IO_OUT,
                      MiracBroker::send_cb, &network_source_ptr_);
    } else {
        g_unix_fd_add(network_->GetHandle(), G_IO_OUT,
                      MiracBroker::connect_cb, &network_source_ptr_);
    }
}

unsigned short MiracBroker::get_host_port() const
{
    return network_->GetHostPort();
}

std::string MiracBroker::get_peer_address() const
{
    return connection_->GetPeerAddress();
}

MiracBroker::MiracBroker (const std::string& listen_port):
    connect_timer_(NULL)
{
    network_source_ptr_ = this;
    connection_source_ptr_ = this;

    network(new MiracNetwork());

    network_->Bind(NULL, listen_port.c_str());
    g_unix_fd_add(network_->GetHandle(), G_IO_IN,
                  MiracBroker::listen_cb, &network_source_ptr_);
}

MiracBroker::MiracBroker(const std::string& peer_address, const std::string& peer_port, uint timeout):
    peer_address_(peer_address),
    peer_port_(peer_port),
    connect_timeout_(timeout)
{
    network_source_ptr_ = this;
    connection_source_ptr_ = this;
    connect_timer_ = g_timer_new();
    try_connect();
}

MiracBroker::~MiracBroker ()
{
    network(NULL);
    connection(NULL);

    if (connect_timer_) {
        g_timer_destroy(connect_timer_);
        connect_timer_ = NULL;
    }

    if (connect_wait_id_ > 0) {
        g_source_remove(connect_wait_id_);
        connect_wait_id_ = 0;
    }
}

void MiracBroker::SendRTSPData(const std::string& data) {
  WFD_VLOG("Sending RTSP message:\n%s", data.c_str());

  if (connection_ && !connection_->Send(data))
      g_unix_fd_add(connection_->GetHandle(), G_IO_OUT,
                    send_cb, &connection_source_ptr_);
}

static gboolean on_timeout(gpointer user_data) {
  TimerCallbackData* data = static_cast<TimerCallbackData*>(user_data);
  data->delegate_->OnTimeout(data->timer_id_);
  delete data;
  return FALSE;
}

void MiracBroker::OnTimeout(uint timer_id) {
  if (std::find(timers_.begin(), timers_.end(), timer_id) != timers_.end())
    Peer()->OnTimerEvent(timer_id);
}

uint MiracBroker::CreateTimer(int seconds) {
  TimerCallbackData* data = new TimerCallbackData(this);
  uint timer_id = g_timeout_add_seconds(
                        seconds,
                        on_timeout,
                        data);
  if (timer_id > 0) {
    data->timer_id_ = timer_id;
    timers_.push_back(timer_id);
  } else {
    delete data;
  }

  return timer_id;
}

void MiracBroker::ReleaseTimer(uint timer_id) {
  if (timer_id > 0) {
    auto it = std::find(timers_.begin(), timers_.end(), timer_id);
    if (it != timers_.end() )
      timers_.erase(it);
  }
}

