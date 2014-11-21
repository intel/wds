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

#include "mirac-gst-test-source.hpp"

MiracGstTestSource::MiracGstTestSource (wfd_test_stream_t wfd_stream_type, std::string hostname, int port)
{
    std::string gst_pipeline;

    std::string hostname_port = (!hostname.empty() ? "host=" + hostname + " ": " ") + (port > 0 ? "port=" + std::to_string(port) : "");

    if (wfd_stream_type == WFD_TEST_BOTH) {
        gst_pipeline = "videotestsrc ! x264enc ! muxer.  audiotestsrc ! avenc_ac3 ! muxer.  mpegtsmux name=muxer ! rtpmp2tpay ! udpsink name=sink " +
            hostname_port;
    } else if (wfd_stream_type == WFD_TEST_AUDIO) {
        gst_pipeline = "audiotestsrc ! avenc_ac3 ! mpegtsmux ! rtpmp2tpay ! udpsink " + hostname_port;
    } else if (wfd_stream_type == WFD_TEST_VIDEO) {
        gst_pipeline = "videotestsrc ! x264enc ! mpegtsmux ! rtpmp2tpay ! udpsink " + hostname_port;
    } else if (wfd_stream_type == WFD_DESKTOP) {
        gst_pipeline = "ximagesrc ! videoconvert ! x264enc tune=zerolatency ! mpegtsmux ! rtpmp2tpay ! udpsink " + hostname_port;
    }

    gst_elem = gst_parse_launch(gst_pipeline.c_str(), NULL);
}

void MiracGstTestSource::SetState(GstState state)
{
    if (gst_elem) {
        gst_element_set_state (gst_elem, state);
    }
}

int MiracGstTestSource::UdpSourcePort()
{
    if (gst_elem == NULL)
        return 0;

    GstElement* sink = NULL;
    gst_bin_get_by_name(GST_BIN(gst_elem), "sink");

    if (sink == NULL)
        return 0;

    gint port = 0;
    g_object_get(sink, "bind-port", &port, NULL);
    return port;
}

MiracGstTestSource::~MiracGstTestSource ()
{
    if (gst_elem) {
        gst_element_set_state (gst_elem, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (gst_elem));
    }
}