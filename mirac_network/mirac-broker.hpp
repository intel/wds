/*
 * This file is part of XXX
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * Contact: Jussi Kukkonen <jussi.kukkonen@intel.com>
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

#include "mirac-network.hpp"
#include "driver.h"

class MiracBrokerObserver
{
    public:
        virtual ~MiracBrokerObserver();
};

class MiracBroker
{
    public:
        MiracBroker ();
        MiracBroker(const std::string& address, const std::string& port);
        virtual ~MiracBroker ();
        unsigned short get_host_port() const;

    protected:
        virtual void got_message(std::shared_ptr<WFD::Message> message) = 0;
        void send(WFD::Message& message) const;
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

        WFD::Driver driver_;
        std::shared_ptr<WFD::Message> message_;

        std::unique_ptr<MiracNetwork> network_;
        std::unique_ptr<MiracNetwork> connection_;
};


#endif  /* MIRAC_BROKER_HPP */

