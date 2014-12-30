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


#ifndef MIRAC_BROKER_HPP
#define MIRAC_BROKER_HPP

#include <glib.h>
#include <memory>
#include <map>
#include <vector>

#include "wfd/public/peer.h"
#include "mirac-network.hpp"

class MiracBrokerObserver
{
    public:
        virtual ~MiracBrokerObserver();
};

class MiracBroker : public wfd::Peer::Delegate
{
    public:
        MiracBroker (const std::string& listen_port);
        MiracBroker(const std::string& peer_address, const std::string& peer_port);
        virtual ~MiracBroker ();
        unsigned short get_host_port() const;
        std::string get_peer_address() const;
        virtual wfd::Peer* Peer() const = 0;
        void OnTimeout(uint timer_id);

    protected:
        // wfd::Peer::Delegate
        virtual void SendRTSPData(const std::string& data) override;
        virtual uint CreateTimer(int seconds);
        virtual void ReleaseTimer(uint timer_id);

        virtual void got_message(const std::string& data) {}
        virtual void on_connected() {};

    private:
        static gboolean send_cb (gint fd, GIOCondition condition, gpointer data_ptr);
        static gboolean receive_cb (gint fd, GIOCondition condition, gpointer data_ptr);
        static gboolean listen_cb (gint fd, GIOCondition condition, gpointer data_ptr);
        static gboolean connect_cb (gint fd, GIOCondition condition, gpointer data_ptr);

        gboolean send_cb (gint fd, GIOCondition condition);
        gboolean receive_cb (gint fd, GIOCondition condition);
        gboolean listen_cb (gint fd, GIOCondition condition);
        gboolean connect_cb (gint fd, GIOCondition condition);

        void handle_body(const std::string msg);
        void handle_header(const std::string msg);

        std::unique_ptr<MiracNetwork> network_;
        std::unique_ptr<MiracNetwork> connection_;
        std::vector<uint> timers_;
};


#endif  /* MIRAC_BROKER_HPP */

