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

#ifndef CONNMAN_CLIENT_H_
#define CONNMAN_CLIENT_H_

#include <memory>
#include <gio/gio.h>

#include "information-element.h"

class ConnmanClient {
    public:
        ConnmanClient(std::unique_ptr<P2P::InformationElementArray> &take_array);
        virtual ~ConnmanClient();

        void set_information_element(std::unique_ptr<P2P::InformationElementArray> &take_array);

    private:
        static void proxy_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);
        static void register_peer_service_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);

        void proxy_cb(GObject *object, GAsyncResult *res);
        void register_peer_service_cb(GObject *object, GAsyncResult *res);
        void register_peer_service();
        void unregister_peer_service();

        GDBusProxy *proxy_;
        std::unique_ptr<P2P::InformationElementArray>array_;
};

#endif // CONNMAN_CLIENT_H_
