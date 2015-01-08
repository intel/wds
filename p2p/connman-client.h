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
#include "connman-peer.h"

class ConnmanClient {
    public:
		class Observer {
			public:
				virtual void on_peers_changed(ConnmanClient *client) {}

			protected:
				virtual ~Observer() {}
		};

        ConnmanClient(std::unique_ptr<P2P::InformationElementArray> &take_array);
        virtual ~ConnmanClient();

        void set_information_element(std::unique_ptr<P2P::InformationElementArray> &take_array);
		void set_observer(Observer* observer) {
			observer_ = observer;
		}

		void scan();

    private:
        static void proxy_signal_cb (GDBusProxy *proxy, const char *sender, const char *signal, GVariant *params, gpointer data_ptr);
        static void proxy_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);
        static void technology_proxy_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);
        static void register_peer_service_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);
        static void scan_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);

        void peers_changed (GVariant *params);
        void proxy_cb(GAsyncResult *res);
        void technology_proxy_cb(GAsyncResult *res);

        void register_peer_service();
        void unregister_peer_service();

        GDBusProxy *proxy_;
        GDBusProxy *technology_proxy_;

		Observer* observer_;
        std::unique_ptr<P2P::InformationElementArray>array_;
		std::map<std::string, std::shared_ptr<P2P::Peer>> peers_;
};

#endif // CONNMAN_CLIENT_H_
