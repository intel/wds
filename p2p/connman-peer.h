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

#include "information-element.h"

#ifndef CONNMAN_PEER_H_
#define CONNMAN_PEER_H_


namespace P2P {

class Peer {
    public:
        Peer(std::string object_path, std::shared_ptr<P2P::InformationElement>);
        virtual ~Peer();

		// tODO notify observer(s) of readiness changes

    private:
        static void proxy_signal_cb (GDBusProxy *proxy, const char *sender, const char *signal, GVariant *params, gpointer data_ptr);
        static void proxy_cb(GObject *object, GAsyncResult *res, gpointer data_ptr);

		void ip_changed (char *ip);
		void state_changed (bool ready);
        void proxy_cb (GAsyncResult *res);

		std::string ip_address_;
		bool ready_;
        GDBusProxy *proxy_;
		std::shared_ptr<P2P::InformationElement> ie_;
};

}
#endif // CONNMAN_PEER_H_