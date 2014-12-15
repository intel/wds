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
#include <gst/gst.h> // gst_init_get_option_group()

#include <iostream>

#include "sink.h"
#include "connman-client.h"


struct SinkAppData {
    std::unique_ptr<Sink> sink;
    std::unique_ptr<ConnmanClient> connman;

    std::string host;
    int port;
};

static gboolean _sig_handler (gpointer data_ptr)
{
    GMainLoop *main_loop = (GMainLoop *) data_ptr;

    g_main_loop_quit(main_loop);

    return G_SOURCE_CONTINUE;
}

static void parse_input_and_call_sink(
    const std::string& command, const std::unique_ptr<Sink> &sink) {
    if (command == "teardown\n") {
        sink->Teardown();
        return;
    }
    if (command == "pause\n") {
        sink->Pause();
        return;
    }
    if (command == "play\n") {
        sink->Play();
        return;
    }
    std::cout << "Received unknown command: " << command << std::endl;
}

static gboolean _user_input_handler (
    GIOChannel* channel, GIOCondition /*condition*/, gpointer data_ptr)
{
    GError* error = NULL;
    char* str = NULL;
    size_t len;
    SinkAppData* data = static_cast<SinkAppData*>(data_ptr);

    switch (g_io_channel_read_line(channel, &str, &len, NULL, &error)) {
    case G_IO_STATUS_NORMAL:
        parse_input_and_call_sink(str, data->sink);
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

static gboolean create_sink (gpointer data_ptr)
{
    SinkAppData* data = static_cast<SinkAppData*>(data_ptr);

    try {
        data->sink.reset(new Sink (data->host.c_str(), data->port));
        std::cout << "Running sink on port "<< data->sink->get_host_port() << std::endl;
        return G_SOURCE_REMOVE;
    } catch (const std::exception &x) {
        std::cout << "Failed to create sink, trying again soon..." << std::endl;
        return G_SOURCE_CONTINUE;
    }
}

int main (int argc, char *argv[])
{
    SinkAppData data;
    gchar* hostname_option = NULL;
    data.port = 7236;

    GOptionEntry main_entries[] =
    {
        { "hostname", 0, 0, G_OPTION_ARG_STRING, &hostname_option, "Specify optional hostname, local host by default", "host"},
        { "rtsp_port", 0, 0, G_OPTION_ARG_INT, &(data.port), "Specify optional RTSP port number, 7236 by default", "rtsp_port"},
        { NULL }
    };

    GOptionContext* context = g_option_context_new ("- WFD sink demo application\n");
    g_option_context_add_main_entries (context, main_entries, NULL);
    g_option_context_add_group (context, gst_init_get_option_group ());

    GError* error = NULL;
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print ("option parsing failed: %s\n", error->message);
        g_option_context_free(context);
        exit (1);
    }
    g_option_context_free(context);

    if (hostname_option) {
        data.host = hostname_option;
        g_free(hostname_option);
    } else {
        data.host ="127.0.0.1";
    }

    GMainLoop *main_loop =  g_main_loop_new(NULL, TRUE);
    g_unix_signal_add(SIGINT, _sig_handler, main_loop);
    g_unix_signal_add(SIGTERM, _sig_handler, main_loop);

    GIOChannel* io_channel = g_io_channel_unix_new (STDIN_FILENO);
    g_io_add_watch(io_channel, G_IO_IN, _user_input_handler, &data);
    g_io_channel_unref(io_channel);

    // Create a information element for a simple WFD Sink
    P2P::InformationElement ie;
    auto sub_element = P2P::new_subelement(P2P::DEVICE_INFORMATION);
    auto dev_info = (P2P::DeviceInformationSubelement*)sub_element;
    // FIXME port number is a lie
    dev_info->session_management_control_port =  htons(7236);
    dev_info->maximum_throughput = htons(50);
    dev_info->field1.device_type = P2P::PRIMARY_SINK;
    dev_info->field1.session_availability = true;
    ie.add_subelement (sub_element);

    std::cout << "Registering Wifi Display on port with IE " << ie.to_string() <<  std::endl;

    // register the P2P service with connman
    auto array = ie.serialize ();
    data.connman.reset(new ConnmanClient (array));

    // Hack while we wait for a better solution:
    // try to create a sink every second and hope the connect succeeds at some point
    // when the source is actually there...
    g_timeout_add_seconds (1, create_sink, &data);

    g_main_loop_run (main_loop);

    g_main_loop_unref (main_loop);

    return 0;
}

