/*
 * This file is part of wysiwidi
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
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

#include "mirac-gst.hpp"

void _set_udp_caps(GstElement *playbin, GstElement *source, gpointer    user_data)
{
    GstCaps* caps = gst_caps_new_simple ("application/x-rtp",
        "media", G_TYPE_STRING, "video",
        "clock-rate", G_TYPE_INT, 1,
        "encoding-name", G_TYPE_STRING, "MP2T",
        NULL);

    g_object_set(source, "caps", caps, NULL);
    gst_caps_unref(caps);
}

MiracGst::MiracGst (wfd_device_t wfd_device, wfd_stream_t wfd_stream, std::string hostname, int port)
{
    std::string gst_pipeline;

    if (wfd_device == WFD_SOURCE) {
        std::string hostname_port = (!hostname.empty() ? "host=" + hostname + " ": " ") + (port > 0 ? "port=" + std::to_string(port) : "");

        if (wfd_stream == WFD_BOTH) {
            gst_pipeline = "videotestsrc ! x264enc ! muxer.  audiotestsrc ! avenc_ac3 ! muxer.  mpegtsmux name=muxer ! rtpmp2tpay ! udpsink " +
                hostname_port;
        } else if (wfd_stream == WFD_AUDIO) {
            gst_pipeline = "audiotestsrc ! avenc_ac3 ! mpegtsmux ! rtpmp2tpay ! udpsink " + hostname_port;
        } else if (wfd_stream == WFD_VIDEO) {
            gst_pipeline = "videotestsrc ! x264enc ! mpegtsmux ! rtpmp2tpay ! udpsink " + hostname_port;
        }
    } else if (wfd_device == WFD_SINK) {
        std::string url =  "udp://" + (!hostname.empty() ? hostname  : "0.0.0.0") + (port > 0 ? ":" + std::to_string(port) : "");
        gst_pipeline = "playbin uri=" + url;
    }

    gst_elem = gst_parse_launch(gst_pipeline.c_str(), NULL);
    if (gst_elem) {
        if (wfd_device == WFD_SINK) {
            g_signal_connect(gst_elem, "source-setup", G_CALLBACK(_set_udp_caps), NULL);
        }
        gst_element_set_state (gst_elem, GST_STATE_PLAYING);
    }
}

int MiracGst::sink_udp_port() {
    if (gst_elem == NULL)
        return 0;

    GstElement* source = NULL;
    g_object_get(gst_elem, "source", &source, NULL);

    if (source == NULL)
        return 0;

    gint port = 0;
    g_object_get(source, "port", &port, NULL);
    return port;
}

MiracGst::~MiracGst ()
{
    if (gst_elem) {
        gst_element_set_state (gst_elem, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (gst_elem));
    }
}
