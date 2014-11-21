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


#include <memory>
#include <algorithm>
#include <assert.h>

#include "mirac-desktop-source.hpp"
#include "reply.h"
#include "options.h"
#include "setup.h"
#include "pause.h"
#include "play.h"
#include "teardown.h"
#include "setparameter.h"
#include "audiocodecs.h"
#include "videoformats.h"
#include "formats3d.h"
#include "clientrtpports.h"
#include "presentationurl.h"
#include "displayedid.h"
#include "coupledsink.h"
#include "i2c.h"
#include "connectortype.h"
#include "standbyresumecapability.h"
#include "getparameter.h"
#include "setparameter.h"

void MiracSource::set_state(MiracSource::State state)
{
    state_ = state;
    if (state == INIT) {
        send_cseq_ = 1;
    }
    std::cout << "** State "<< state_ << std::endl;
}

void MiracSource::set_session (std::string session)
{
    session_ = session;
}

void MiracSource::set_rtp_ports(unsigned short port_0, unsigned short port_1)
{
    rtp_port_0_ = port_0;
    rtp_port_1_ = port_1;
}

void MiracSource::handle_m2_options (std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    std::vector<WFD::Method> methods;

    set_state (CAPABILITY_NEGOTIATION);

    methods.push_back(WFD::Method::ORG_WFA_WFD_1_0);
    methods.push_back(WFD::Method::GET_PARAMETER);
    methods.push_back(WFD::Method::SET_PARAMETER);
    methods.push_back(WFD::Method::SETUP);
    methods.push_back(WFD::Method::TEARDOWN);
    methods.push_back(WFD::Method::PLAY);
    methods.push_back(WFD::Method::PAUSE);
    reply.header().set_supported_methods(methods);
    reply.header().set_cseq (message->header().cseq());

    send (reply);

    // Send M3 GET_PARAMETER
    expected_reply_ = WFD::Method::GET_PARAMETER;
    WFD::GetParameter m3("rtsp://localhost/wfd1.0");
    m3.header().set_cseq (send_cseq_++);

    m3.payload().add_get_parameter_property(WFD::WFD_AUDIO_CODECS);
    m3.payload().add_get_parameter_property(WFD::WFD_VIDEO_FORMATS);
    m3.payload().add_get_parameter_property(WFD::WFD_CLIENT_RTP_PORTS);

    send (m3);
}

void MiracSource::handle_m1_options_reply (std::shared_ptr<WFD::Reply> reply)
{
    // not expecting anything
    expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

    // Ensure M1 OPTIONS reply is valid
    if (reply->response_code() != 200)
        return;

    auto methods = reply->header().supported_methods();

    if (std::find (methods.begin(), methods.end(), WFD::Method::ORG_WFA_WFD_1_0) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::GET_PARAMETER) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::SET_PARAMETER) == methods.end()) {

        std::cout << "M2 OPTIONS reply with insufficient methods" << std::endl;
    }

}

void MiracSource::handle_m3_get_parameters_reply (std::shared_ptr<WFD::Reply> reply)
{
    // Ensure M3 GET_PARAMETERS reply is valid
    if (reply->response_code() != 200)
        return;

    auto video_formats = std::static_pointer_cast<WFD::VideoFormats>(reply->payload().get_property (WFD::PropertyType::WFD_VIDEO_FORMATS));
    if (video_formats == NULL) {
        std::cout << "** GET_PARAMETER: missing wfd_video_formats in response" << std::endl;
        return;
    }
    // technically, we're suppposed to pick and set a video format here,
    // but the task seems so insanely tedious that for now let's just skip it, and
    // hope that sinks don't care (our sink doesn't)

    auto audio_codecs = std::static_pointer_cast<WFD::AudioCodecs>(reply->payload().get_property (WFD::PropertyType::WFD_AUDIO_CODECS));
    if (audio_codecs == NULL) {
        std::cout << "** GET_PARAMETER: missing wfd_audio_codecs in response" << std::endl;
        return;
    }
    // we can ignore the audio codecs because our stream is video-only

    auto rtp_ports = std::static_pointer_cast<WFD::ClientRtpPorts>(reply->payload().get_property (WFD::PropertyType::WFD_CLIENT_RTP_PORTS));
    if (audio_codecs == NULL) {
        std::cout << "** GET_PARAMETER: missing wfd_client_rtp_ports in response" << std::endl;
        return;
    }
    set_rtp_ports(rtp_ports->rtp_port_0(), rtp_ports->rtp_port_1());

    // Send M4 SET_PARAMETER
    expected_reply_ = WFD::Method::SET_PARAMETER;
    WFD::SetParameter m4("rtsp://localhost/wfd1.0");
    m4.header().set_cseq (send_cseq_++);

    std::shared_ptr<WFD::Property> rtp_ports_set(new WFD::ClientRtpPorts(rtp_ports->rtp_port_0(), rtp_ports->rtp_port_1()));
    m4.payload().add_property(rtp_ports_set);
    // hopefully no one cares about the IP address; they shouldn't
    std::shared_ptr<WFD::Property> presentation_url_set(new WFD::PresentationUrl("rtsp://127.0.0.1/wfd1.0/streamid=0",""));
    m4.payload().add_property(presentation_url_set);

    send (m4);

}

void MiracSource::send_wfd_trigger_method(WFD::TriggerMethod::Method method)
{
    expected_reply_ = WFD::Method::SET_PARAMETER;
    WFD::SetParameter m5("rtsp://localhost/wfd1.0");
    m5.header().set_cseq (send_cseq_++);

    std::shared_ptr<WFD::Property> wfd_trigger_method(new WFD::TriggerMethod(method));
    m5.payload().add_property(wfd_trigger_method);

    send(m5);
}

void MiracSource::handle_m4_set_parameters_reply (std::shared_ptr<WFD::Reply> reply)
{
    // Ensure M4 SET_PARAMETERS reply is valid
    if (reply->response_code() != 200)
        return;

    // send M5 wfd_trigger_method: SETUP
    set_state(RTSP_SESSION_ESTABLISHMENT);

    send_wfd_trigger_method(WFD::TriggerMethod::SETUP);
}

void MiracSource::handle_m5_set_parameters_reply (std::shared_ptr<WFD::Reply> reply)
{
    // not expecting anything
    expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

    // Ensure M5 SET_PARAMETERS reply is valid
    if (reply->response_code() != 200)
        return;

    // otherwise do nothing; the sink will send a SETUP/PLAY/PAUSE/TEARDOWN command
}

void MiracSource::handle_m6_setup (std::shared_ptr<WFD::Message> message)
{
    unsigned int client_port = message->header().transport().client_port();

    // set up gstreamer pipeline with client_port, but do not play yet
    gst_pipeline.reset(new MiracGstTestSource(WFD_DESKTOP, get_peer_address(), client_port));

    // also get the source udp port from gstreamer
    unsigned int server_port = gst_pipeline->UdpSourcePort();

    // FIXME: generate random session id
    std::string session("abcdefgh");
    set_session(session);

    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    auto transport = new WFD::TransportHeader();
    transport->set_client_port(client_port);
    transport->set_server_port(server_port);
    reply.header().set_transport(transport);
    reply.header().set_session(session);

    send (reply);
    set_state (WFD_SESSION_ESTABLISHMENT);
}

void MiracSource::handle_m7_play (std::shared_ptr<WFD::Message> message)
{
    // instruct the gstreamer pipeline to start playing
    gst_pipeline->SetState(GST_STATE_PLAYING);

    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    send (reply);
    set_state (WFD_SESSION_PLAYING);
}

void MiracSource::handle_m9_pause (std::shared_ptr<WFD::Message> message)
{
    // instruct the gstreamer pipeline to pause
    gst_pipeline->SetState(GST_STATE_PAUSED);

    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    send (reply);
    set_state (WFD_SESSION_PAUSED);
}

void MiracSource::handle_m8_teardown (std::shared_ptr<WFD::Message> message)
{
    // instruct the gstreamer pipeline to stop
    gst_pipeline->SetState(GST_STATE_READY);

    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    send (reply);
    set_state (INIT);
}

bool MiracSource::validate_message_sequence(std::shared_ptr<WFD::Message> message) const
{
    if (message->type() == WFD::Message::MessageTypeReply) {
        // Ensure sent message sequence
        if (message->header().cseq() != send_cseq_ - 1) {
            std::cout << "** Out-of-order reply CSeq, dropping reply" << std::endl;
            return false;
        }
        if (expected_reply_ == WFD::Method::ORG_WFA_WFD_1_0) {
            std::cout << "** Not expecting reply, dropping reply" << std::endl;
            return false;
        }
    } else {
        // Ensure received message sequence
        if (message->type() != WFD::Message::MessageTypeOptions &&
            message->header().cseq() != receive_cseq_ + 1) {

            WFD::Reply reply(400);
            reply.header().set_cseq (message->header().cseq());
            send (reply);

            std::cout << "** Out-of-order message CSeq " << message->header().cseq() << std::endl;
            return false;
        }

        if (expected_reply_ != WFD::Method::ORG_WFA_WFD_1_0) {
            WFD::Reply reply(400);
            reply.header().set_cseq (message->header().cseq());
            send (reply);

            std::cout << "** Expected a reply";
            return false;
        }
    }

    return true;
}

void MiracSource::handle_get_parameter(std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());
    send(reply);
}

void MiracSource::handle_set_parameter(std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());
    send(reply);
}

void MiracSource::got_message(std::shared_ptr<WFD::Message> message)
{
    std::cout << "** got msg: "  << std::endl << message->to_string() << std::endl;

    if (!validate_message_sequence (message))
        return;

    if (message->type() == WFD::Message::MessageTypeOptions)
        receive_cseq_ = message->header().cseq();
    else if (message->type() != WFD::Message::MessageTypeReply)
        receive_cseq_++;

    switch (message->type()) {
        case WFD::Message::MessageTypeOptions:
            if (state_ == CAPABILITY_NEGOTIATION)
                handle_m2_options(message);
            else 
                std::cout << "** Unexpected OPTIONS" << std::endl;
            break;
        case WFD::Message::MessageTypeGetParameter:
            if (state_ >= CAPABILITY_NEGOTIATION)
                handle_get_parameter(message);
            else 
                std::cout << "** Unexpected GET_PARAMETER" << std::endl;
            break;
        case WFD::Message::MessageTypeSetParameter:
        {
            auto set_param = std::static_pointer_cast<WFD::SetParameter>(message);
            if (state_ >= CAPABILITY_NEGOTIATION ) {
                handle_set_parameter (message);
            } else {
                std::cout << "** Unexpected SET_PARAMETER" << std::endl;
            }
            break;
        }
        case WFD::Message::MessageTypeSetup:
        {
            if (state_ == RTSP_SESSION_ESTABLISHMENT) {
                handle_m6_setup (message);
            } else {
                std::cout << "** Unexpected SETUP" << std::endl;
            }
            break;
        }
        case WFD::Message::MessageTypePlay:
        {
            if (state_ >= RTSP_SESSION_ESTABLISHMENT) {
                handle_m7_play (message);
            } else {
                std::cout << "** Unexpected PLAY" << std::endl;
            }
            break;
        }
        case WFD::Message::MessageTypePause:
        {
            if (state_ >= RTSP_SESSION_ESTABLISHMENT) {
                handle_m9_pause (message);
            } else {
                std::cout << "** Unexpected PAUSE" << std::endl;
            }
            break;
        }
        case WFD::Message::MessageTypeTeardown:
        {
            if (state_ >= RTSP_SESSION_ESTABLISHMENT) {
                handle_m8_teardown (message);
            } else {
                std::cout << "** Unexpected TEARDOWN" << std::endl;
            }
            break;
        }
        case WFD::Message::MessageTypeReply:
            if (state_ == CAPABILITY_NEGOTIATION &&
                expected_reply_ == WFD::Method::OPTIONS) {
                handle_m1_options_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else if (state_ == CAPABILITY_NEGOTIATION &&
                expected_reply_ == WFD::Method::GET_PARAMETER) {
                handle_m3_get_parameters_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else if (state_ == CAPABILITY_NEGOTIATION &&
                expected_reply_ == WFD::Method::SET_PARAMETER) {
                handle_m4_set_parameters_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else if (state_ >= RTSP_SESSION_ESTABLISHMENT &&
                expected_reply_ == WFD::Method::SET_PARAMETER) {
                handle_m5_set_parameters_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else {
                std::cout << "** Unexpected reply" << std::endl;
            }
            break;
        default:
            std::cout << "** Method not implemented" << std::endl;
            break;
    }
}

void MiracSource::on_connected()
{
    set_state (CAPABILITY_NEGOTIATION);

    // Send M1 OPTIONS
    expected_reply_ = WFD::Method::OPTIONS;
    WFD::Options m1("*");
    m1.header().set_cseq (send_cseq_++);
    m1.header().set_require_wfd_support (true);
    send (m1);
}

MiracSource::MiracSource(int rtsp_port)
    : MiracBroker(std::to_string(rtsp_port)),
      send_cseq_(0),
      receive_cseq_(0) {

}

MiracSource::~MiracSource()
{
}

void MiracSource::Teardown() {
    std::cout << "** teardown" << std::endl;

    send_wfd_trigger_method(WFD::TriggerMethod::TEARDOWN);
}

void MiracSource::Play() {
    std::cout << "** play" << std::endl;

    send_wfd_trigger_method(WFD::TriggerMethod::PLAY);
}

void MiracSource::Pause() {
    std::cout << "** pause" << std::endl;

    send_wfd_trigger_method(WFD::TriggerMethod::PAUSE);
}

