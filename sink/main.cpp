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

#include <glib.h>
#include <glib-unix.h>
#include <netinet/in.h> // htons()

#include "mirac-sink.hpp"
#include "connman-client.h"

static gboolean _sig_handler (gpointer data_ptr)
{
    GMainLoop *main_loop = (GMainLoop *) data_ptr;

    g_main_loop_quit(main_loop);

    return G_SOURCE_CONTINUE;
}

int main (int argc, char *argv[])
{
    GMainLoop *main_loop =  g_main_loop_new(NULL, TRUE);
    g_unix_signal_add(SIGINT, _sig_handler, main_loop);
    g_unix_signal_add(SIGTERM, _sig_handler, main_loop);

    auto sink = new MiracSink (); 
    std::cout << "Running sink on port "<< sink->get_host_port() << std::endl;

    // Create a information element for a simple WFD Sink
    P2P::InformationElement ie;
    auto sub_element = P2P::new_subelement(P2P::DEVICE_INFORMATION);
    auto dev_info = (P2P::DeviceInformationSubelement*)sub_element;
    dev_info->session_management_control_port =  htons(sink->get_host_port());
    dev_info->maximum_throughput = htons(50);
    dev_info->field1.device_type = P2P::PRIMARY_SINK;
    dev_info->field1.session_availability = true;
    ie.add_subelement (sub_element);

    std::cout << "Registering Wifi Display on port with IE " << ie.to_string() <<  std::endl;

    // register the P2P service with connman
    auto array = ie.serialize ();
    ConnmanClient client (array);

    g_main_loop_run (main_loop);

    g_main_loop_unref (main_loop);
    delete sink;

    return 0;
}

