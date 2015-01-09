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
#include <memory>

#include "connman-client.h"

/* static C callback */
void ConnmanClient::proxy_signal_cb (GDBusProxy *proxy, const char *sender, const char *signal, GVariant *params, gpointer data_ptr)
{
    if (g_strcmp0(signal, "PeersChanged") != 0)
        return;

    auto client = reinterpret_cast<ConnmanClient*> (data_ptr);
    client->peers_changed (params);
}

void ConnmanClient::register_peer_service_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    GError *error = NULL;
    GDBusProxy *proxy = G_DBUS_PROXY (object);

    g_dbus_proxy_call_finish (proxy, res, &error);
    if (error) {
        std::cout << "register error " << error->message << std::endl;
        g_clear_error (&error);
        return;
    }
}

/* static C callback */
void ConnmanClient::scan_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    GError *error = NULL;
    GDBusProxy *proxy = G_DBUS_PROXY (object);

    g_dbus_proxy_call_finish (proxy, res, &error);
    if (error) {
        std::cout << "scan error " << error->message << std::endl;
        g_clear_error (&error);
        return;
    }

    std::cout << "* scan started "<< std::endl;
}

/* static C callback */
void ConnmanClient::proxy_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    auto client = reinterpret_cast<ConnmanClient*> (data_ptr);
    client->proxy_cb (res);
}

/* static C callback */
void ConnmanClient::technology_proxy_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    auto client = reinterpret_cast<ConnmanClient*> (data_ptr);
    client->technology_proxy_cb (res);
}

void ConnmanClient::peers_changed (GVariant *params)
{
    GVariantIter *added, *removed, *props;
    char *path;

    g_variant_get (params, "(a(oa{sv})ao)", &added, &removed);
    while (g_variant_iter_loop (added, "(&oa{sv})", &path, &props)) {
        char *name;
        GVariant *val;
		while (g_variant_iter_loop (props, "{&sv}", &name, &val)) {
            if (g_strcmp0 (name, "Services") == 0) {
                GVariantIter *service_array, *services;
                GVariant *spec_val;

                g_variant_get (val, "a(a{sv})", &service_array);
                while (g_variant_iter_loop (service_array, "(a{sv})", &services)) {
    	            while (g_variant_iter_loop (services, "{sv}", &name, &spec_val)) {
                        if (g_strcmp0 (name, "WiFiDisplayIEs") == 0) {
                            uint8_t *bytes;
							gsize length;

                            bytes = (uint8_t*)g_variant_get_fixed_array (spec_val, &length, 1);
							std::unique_ptr<P2P::InformationElementArray> array
									(new P2P::InformationElementArray(length, bytes));
							auto ie = std::make_shared<P2P::InformationElement>(array);

							peers_[path] = std::make_shared<P2P::Peer>(std::string(path), ie);
							if (observer_){
								observer_->on_peer_added(this, peers_[path]);
							}
                       }
                   }
               }
           }
        }
    }


    while (g_variant_iter_loop (removed, "o", &path)) {
		/* TODO call on_peer_removed() */
        if (peers_.erase (path) > 0) {
			std::cout << "removed peer " << path << std::endl;
		}
    }

    g_variant_iter_free (added);
    g_variant_iter_free (removed);
}

void ConnmanClient::register_peer_service ()
{
    GVariantBuilder builder;
    g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add (&builder, "{sv}",
                           "WiFiDisplayIEs",
                           g_variant_new_fixed_array (G_VARIANT_TYPE_BYTE,
                                                      array_->bytes,
                                                      array_->length,
                                                      1));

    g_dbus_proxy_call (proxy_,
                       "RegisterPeerService",
                       g_variant_new ("(a{sv}b)", &builder, TRUE),
                       G_DBUS_CALL_FLAGS_NONE,
                       -1,
                       NULL,
                       ConnmanClient::register_peer_service_cb,
                       this);
}

void ConnmanClient::unregister_peer_service ()
{
    GVariantBuilder builder;
    g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add (&builder, "{sv}",
                           "WiFiDisplayIEs",
                           g_variant_new_fixed_array (G_VARIANT_TYPE_BYTE,
                                                      array_->bytes,
                                                      array_->length,
                                                      1));

    g_dbus_proxy_call (proxy_,
                       "UnregisterPeerService",
                       g_variant_new ("(a{sv}b)", &builder, TRUE),
                       G_DBUS_CALL_FLAGS_NONE,
                       -1,
                       NULL,
                       ConnmanClient::register_peer_service_cb,
                       this);
}

void ConnmanClient::proxy_cb (GAsyncResult *result)
{
    GError *error = NULL;

    proxy_ = g_dbus_proxy_new_for_bus_finish(result, &error);
    if (error) {
        std::cout << "proxy error "<< std::endl;
        g_clear_error (&error);
        return;
    }

    g_signal_connect (proxy_, "g-signal",
                      G_CALLBACK (ConnmanClient::proxy_signal_cb), this);
    /* TODO run GetPeers just in case the list is up to date already */

    register_peer_service();

	if(technology_proxy_ && observer_)
		observer_->on_initialized(this);
}

void ConnmanClient::technology_proxy_cb (GAsyncResult *result)
{
    GError *error = NULL;

    technology_proxy_ = g_dbus_proxy_new_for_bus_finish(result, &error);
    if (error) {
        std::cout << "tech proxy error "<< std::endl;
        g_clear_error (&error);
    }

	if(proxy_ && observer_)
		observer_->on_initialized(this);
}

ConnmanClient::ConnmanClient(std::unique_ptr<P2P::InformationElementArray> &take_array):
	ConnmanClient(take_array, NULL) {}

ConnmanClient::ConnmanClient(std::unique_ptr<P2P::InformationElementArray> &take_array, Observer *observer):
    proxy_(NULL),
    observer_(observer),
    array_(std::move(take_array))
{
    g_dbus_proxy_new_for_bus (G_BUS_TYPE_SYSTEM,
                              G_DBUS_PROXY_FLAGS_NONE,
                              NULL,
                              "net.connman",
                              "/",
                              "net.connman.Manager",
                              NULL,
                              ConnmanClient::proxy_cb,
                              this);


	/* TODO should get the p2p object path
	 * by watching Manager.TechnologyAdded/TechnologyRemoved */
    g_dbus_proxy_new_for_bus (G_BUS_TYPE_SYSTEM,
                              G_DBUS_PROXY_FLAGS_NONE,
                              NULL,
                              "net.connman",
                              "/net/connman/technology/p2p",
                              "net.connman.Technology",
                              NULL,
                              ConnmanClient::technology_proxy_cb,
                              this);

}

ConnmanClient::~ConnmanClient()
{
    if (proxy_)
        g_clear_object (&proxy_);
    if (technology_proxy_)
        g_clear_object (&technology_proxy_);
}

void ConnmanClient::set_information_element(std::unique_ptr<P2P::InformationElementArray> &take_array)
{
    unregister_peer_service();
    array_ = std::move (take_array);
    register_peer_service();
}

void ConnmanClient::scan()
{
    g_dbus_proxy_call (technology_proxy_,
                       "Scan",
                       NULL,
                       G_DBUS_CALL_FLAGS_NONE,
                       -1,
                       NULL,
                       ConnmanClient::scan_cb,
                       this);
}
