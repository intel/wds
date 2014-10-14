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

#include "connman-client.h"

/* static C callback */
void ConnmanClient::register_peer_service_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    auto client = reinterpret_cast<ConnmanClient*> (data_ptr);
    client->register_peer_service_cb (object, res);
}

/* static C callback */
void ConnmanClient::proxy_cb (GObject *object, GAsyncResult *res, gpointer data_ptr)
{
    auto client = reinterpret_cast<ConnmanClient*> (data_ptr);
    client->proxy_cb (object, res);
}

void ConnmanClient::register_peer_service_cb (GObject *object, GAsyncResult *res)
{
    GError *error = NULL;
    GDBusProxy *proxy = G_DBUS_PROXY (object);

    g_dbus_proxy_call_finish(proxy, res, &error);
    if (error) {
        std::cout << "register error " << error->message<< std::endl;
        g_clear_error (&error);
        return;
    }

    std::cout << "* registered peer service "<< std::endl;
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

void ConnmanClient::proxy_cb (GObject *object, GAsyncResult *result)
{
    GError *error = NULL;

    proxy_ = g_dbus_proxy_new_for_bus_finish(result, &error);
    if (error) {
        std::cout << "proxy error "<< std::endl;
        g_clear_error (&error);
        return;
    }

    register_peer_service();
}

ConnmanClient::ConnmanClient(std::unique_ptr<P2P::InformationElementArray> &take_array):
    proxy_(NULL),
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
}

ConnmanClient::~ConnmanClient()
{
    if (proxy_)
        g_clear_object (&proxy_);
}

void ConnmanClient::set_information_element(std::unique_ptr<P2P::InformationElementArray> &take_array)
{
    unregister_peer_service();
    array_ = std::move (take_array);
    register_peer_service();
}
