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

#include <iostream>
#include <gio/gio.h>

#include "connman-peer.h"

namespace P2P {

/* static C callback */
void Peer::proxy_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    auto client = reinterpret_cast<Peer*> (data_ptr);
    client->proxy_cb (res);
}

/* static C callback */
void Peer::proxy_signal_cb (GDBusProxy *proxy, const char *sender, const char *signal, GVariant *params, gpointer data_ptr)
{
	GVariant *property;
	char *name, *local, *remote;
    auto peer = reinterpret_cast<Peer*> (data_ptr);

    if (g_strcmp0(signal, "PropertyChanged") != 0)
        return;

    std::cout << "DBG: peer property changed " << std::endl;
    g_variant_get (params, "(sv)", &name, &property);

	if (g_strcmp0(name, "State") == 0) {
	    const char *state = g_variant_get_string (property, NULL); 

		peer->state_changed (g_strcmp0 (state, "ready") == 0);
	} else if (g_strcmp0(name, "IPv4") == 0) {
		std::cout << "DBG: peer ipv4 changed " << std::endl;
		g_variant_get (property, "({ss})", &local, &remote);

		peer->ip_changed (remote);
	}
}

/* static C callback */
void Peer::connect_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    GError *error = NULL;
    GDBusProxy *proxy = G_DBUS_PROXY (object);

    g_dbus_proxy_call_finish (proxy, res, &error);
    if (error) {
        std::cout << "connect error " << error->message << std::endl;
        g_clear_error (&error);
        return;
    }

    std::cout << "* connected "<< std::endl;
}

/* static C callback */
void Peer::disconnect_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    GError *error = NULL;
    GDBusProxy *proxy = G_DBUS_PROXY (object);

    g_dbus_proxy_call_finish (proxy, res, &error);
    if (error) {
        std::cout << "disconnect error " << error->message << std::endl;
        g_clear_error (&error);
        return;
    }

    std::cout << "* disconnected "<< std::endl;
}

void Peer::proxy_cb (GAsyncResult *result)
{
    GError *error = NULL;

    std::cout << "Registering proxy for peer..." << std::endl;

    proxy_ = g_dbus_proxy_new_for_bus_finish(result, &error);
    if (error) {
        std::cout << "Peer proxy error "<< std::endl;
        g_clear_error (&error);
        return;
    }

    g_signal_connect (proxy_, "g-signal",
                      G_CALLBACK (Peer::proxy_signal_cb), this);

    /* TODO check the ip address in case it's up to date already */
}

void Peer::ip_changed (char *ip)
{
	std::string new_ip(ip);

	if (new_ip.compare (ip_address_) == 0)
		return;
	
	ip_address_ = new_ip;

	if (!observer_)
		return;

	if (ip_address_.empty() && ready_) {
		observer_->on_state_changed(this);
	} else if (!ip_address_.empty() && ready_) {
		observer_->on_state_changed(this);
	}
}

void Peer::state_changed (bool ready)
{
	if (ready_ == ready)
		return;
	
	ready_ = ready;

	if (!observer_)
		return;

	if (!ready_ && !ip_address_.empty())
		observer_->on_state_changed(this);
	if (ready_ && !ip_address_.empty()) {
		observer_->on_state_changed(this);
	}
}

Peer::Peer(const std::string& object_path, std::shared_ptr<P2P::InformationElement> ie):
    ie_(ie)
{
    g_dbus_proxy_new_for_bus (G_BUS_TYPE_SYSTEM,
                              G_DBUS_PROXY_FLAGS_NONE,
                              NULL,
                              "net.connman",
                              object_path.c_str(),
                              "net.connman.Peer",
                              NULL,
                              Peer::proxy_cb,
                              this);
}

void Peer::connect()
{
    g_dbus_proxy_call (proxy_,
                       "Connect",
                       NULL,
                       G_DBUS_CALL_FLAGS_NONE,
                       60 * 1000, // is 1 minute too long?
                       NULL,
                       Peer::connect_cb,
                       this);
}

void Peer::disconnect()
{
    g_dbus_proxy_call (proxy_,
                       "Disconnect",
                       NULL,
                       G_DBUS_CALL_FLAGS_NONE,
                       -1,
                       NULL,
                       Peer::disconnect_cb,
                       this);
}


Peer::~Peer()
{
    if (proxy_)
        g_clear_object (&proxy_);
}

}
