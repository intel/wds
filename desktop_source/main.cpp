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
#include <iostream>

#include "mirac_broker_source.h"
#include "mirac-gst-test-source.hpp"

#include "connman-client.h"

#include "wfd/public/source.h"

struct SourceAppData {
    std::unique_ptr<MiracBrokerSource> source;
    std::unique_ptr<ConnmanClient> connman;

    int port;
};

static gboolean _sig_handler (gpointer data_ptr)
{
    GMainLoop *main_loop = (GMainLoop *) data_ptr;

    g_main_loop_quit(main_loop);

    return G_SOURCE_CONTINUE;
}

static void parse_input_and_call_source(
    const std::string& command, const std::unique_ptr<MiracBrokerSource> &source) {
    bool status = true;
    if (command == "teardown\n") {
      status = source->wfd_source()->Teardown();
    } else if (command == "pause\n") {
      status = source->wfd_source()->Pause();
    } else if (command == "play\n") {
      status = source->wfd_source()->Play();
    } else {
      std::cout << "Received unknown command: " << command << std::endl;
    }
    if (!status)
      std::cout << "This command cannot be executed now." << std::endl;
}

static gboolean _user_input_handler (
    GIOChannel* channel, GIOCondition /*condition*/, gpointer data_ptr)
{
    GError* error = NULL;
    char* str = NULL;
    size_t len;
    SourceAppData* data = static_cast<SourceAppData*>(data_ptr);

    switch (g_io_channel_read_line(channel, &str, &len, NULL, &error)) {
    case G_IO_STATUS_NORMAL:
        parse_input_and_call_source(str, data->source);
        g_free(str);
        return true;
    case G_IO_STATUS_ERROR:
        std::cout << "User input error: " << error->message << std::endl;
        g_error_free(error);
        return false;
    case G_IO_STATUS_EOF:
    case G_IO_STATUS_AGAIN:
        return true;
    default:
        return false;
    }
    return false;
}

int main (int argc, char *argv[])
{
    SourceAppData data;
    data.port = 7236;

    GOptionEntry main_entries[] =
    {
        { "rtsp_port", 0, 0, G_OPTION_ARG_INT, &(data.port), "Specify optional RTSP port number, 7236 by default", "rtsp_port"},
        { NULL }
    };

    GOptionContext* context = g_option_context_new ("- WFD desktop source demo application\n");
    g_option_context_add_main_entries (context, main_entries, NULL);
    g_option_context_add_group (context, gst_init_get_option_group ());

    GError* error = NULL;
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print ("option parsing failed: %s\n", error->message);
        g_option_context_free(context);
        exit (1);
    }
    g_option_context_free(context);

    GMainLoop *main_loop =  g_main_loop_new(NULL, TRUE);
    g_unix_signal_add(SIGINT, _sig_handler, main_loop);
    g_unix_signal_add(SIGTERM, _sig_handler, main_loop);

    GIOChannel* io_channel = g_io_channel_unix_new (STDIN_FILENO);
    g_io_add_watch(io_channel, G_IO_IN, _user_input_handler, &data);
    g_io_channel_unref(io_channel);

    // Create a information element for a simple WFD Source
    // FIXME: is this correct? maybe we're supposed to actively scan for
    // WFD sinks instead
    P2P::InformationElement ie;
    auto sub_element = P2P::new_subelement(P2P::DEVICE_INFORMATION);
    auto dev_info = (P2P::DeviceInformationSubelement*)sub_element;
    dev_info->session_management_control_port =  htons(data.port);
    dev_info->maximum_throughput = htons(50);
    dev_info->field1.device_type = P2P::SOURCE;
    dev_info->field1.session_availability = true;
    ie.add_subelement (sub_element);

    std::cout << "Registering Wifi Source on port with IE " << ie.to_string() <<  std::endl;

    // register the P2P service with connman
    auto array = ie.serialize ();
    data.connman.reset(new ConnmanClient (array));
    data.source.reset(new MiracBrokerSource(data.port));
    g_main_loop_run (main_loop);

    g_main_loop_unref (main_loop);

    return 0;
}

