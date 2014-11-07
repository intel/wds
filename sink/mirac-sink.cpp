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

#include "mirac-sink.hpp"
#include "reply.h"
#include "options.h"
#include "setup.h"
#include "play.h"
#include "teardown.h"
#include "setparameter.h"
#include "audiocodecs.h"
#include "videoformats.h"
#include "formats3d.h"
#include "clientrtpports.h"
#include "presentationurl.h"

#include "mirac-gst.hpp"


MiracSink::SetParameterType MiracSink::get_method(std::shared_ptr<WFD::SetParameter> set_param)
{
    try {
        auto method = set_param->payload().get_property (WFD::PropertyType::WFD_TRIGGER_METHOD);
        auto trigger_method = std::static_pointer_cast<WFD::TriggerMethod> (method);
        switch (trigger_method->method()) {
            case WFD::TriggerMethod::SETUP:
                return M5_TRIGGER_SETUP;
            case WFD::TriggerMethod::PLAY:
                return M5_TRIGGER_PLAY;
            case WFD::TriggerMethod::PAUSE:
                return M5_TRIGGER_PAUSE;
            case WFD::TriggerMethod::TEARDOWN:
                return M5_TRIGGER_TEARDOWN;
            default:
                assert(false);
        }
    } catch (std::out_of_range) {
        return M4;
    }
}


void MiracSink::set_state(MiracSink::State state)
{
    state_ = state;
    std::cout << "** State "<< state_ << std::endl;
}

void MiracSink::set_presentation_url (std::string url)
{
    presentation_url_ = url;
    std::cout << "** Got url "<< presentation_url_ << std::endl;
}

void MiracSink::set_session (std::string session)
{
    session_ = session;
    std::cout << "** Got session "<< session_ << std::endl;
}

void MiracSink::handle_m1_options (std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    std::vector<WFD::Method> methods;

    set_state (CAPABILITY_NEGOTIATION);

    methods.push_back(WFD::Method::ORG_WFA_WFD_1_0);
    methods.push_back(WFD::Method::GET_PARAMETER);
    methods.push_back(WFD::Method::SET_PARAMETER);
    reply.header().set_supported_methods(methods);
    reply.header().set_cseq (message->header().cseq());

    send (reply);

    // Send M2 OPTIONS
    expected_reply_ = WFD::Method::OPTIONS;
    WFD::Options m2("*");
    m2.header().set_cseq (send_cseq_++);
    m2.header().set_require_wfd_support (true);
    send (m2);
}

void MiracSink::handle_m2_options_reply (std::shared_ptr<WFD::Reply> reply)
{
    // not expecting anything
    expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

    // Ensure M2 OPTIONS reply is valid
    if (reply->response_code() != 200)
        return;

    auto methods = reply->header().supported_methods();

    if (std::find (methods.begin(), methods.end(), WFD::Method::ORG_WFA_WFD_1_0) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::GET_PARAMETER) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::SET_PARAMETER) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::SETUP) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::PLAY) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::TEARDOWN) == methods.end() ||
        std::find (methods.begin(), methods.end(), WFD::Method::PAUSE) == methods.end()) {

        std::cout << "M2 OPTIONS reply with insufficient methods" << std::endl;
    }

}

void MiracSink::handle_m3_get_parameter (std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    auto props = message->payload().get_parameter_properties();
    for (auto it = props.begin(); it != props.end(); it++) {
        std::shared_ptr<WFD::Property> new_prop;
        if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_AUDIO_CODECS]){
            // declare that we support absolutely everything, let gstreamer deal with it
            auto codec_lpcm = new WFD::AudioCodec (WFD::AudioFormat::LPCM, WFD::AudioFormat::Modes(3), 0);
            auto codec_aac = new WFD::AudioCodec (WFD::AudioFormat::AAC, WFD::AudioFormat::Modes(15), 0);
            auto codec_ac3 = new WFD::AudioCodec (WFD::AudioFormat::AC3, WFD::AudioFormat::Modes(7), 0);
            auto codec_list = std::vector<WFD::AudioCodec>();
            codec_list.push_back(*codec_lpcm);
            codec_list.push_back(*codec_aac);
            codec_list.push_back(*codec_ac3);
            new_prop.reset(new WFD::AudioCodecs(codec_list));
            reply.payload().add_property(new_prop);
        } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_VIDEO_FORMATS]){
            auto codec_list = WFD::H264Codecs();
            // again, declare that we support absolutely everything, let gstreamer deal with it
            //auto codec_cbp = new WFD::H264Codec(1, 16. ...
            //auto codec_chp = new WFD::H264Codec(2, 16. ...
            //codec_list.push_back(*codec_cbp);
            //codec_list.push_back(*codec_chp);
            new_prop.reset(new WFD::VideoFormats(64 , 0, codec_list)); // 64 should mean 1920x1080p24
            reply.payload().add_property(new_prop);
        } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_3D_FORMATS]){
            new_prop.reset(new WFD::Formats3d());
            reply.payload().add_property(new_prop);
        } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_CLIENT_RTP_PORTS]){
            new_prop.reset(new WFD::ClientRtpPorts(9999, 0));
            reply.payload().add_property(new_prop);
        } else {
            std::cout << "** GET_PARAMETER: Property not supported" << std::endl;
        }
    }

    send (reply);
}

void MiracSink::handle_m4_set_parameter (std::shared_ptr<WFD::Message> message, bool initial)
{
    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    auto props = message->payload().properties();

    if (initial) {
        auto prop = props.find (WFD::PropertyName::name[WFD::PropertyType::WFD_CLIENT_RTP_PORTS]);
        if (prop == props.end()) {
            reply.set_response_code (400);
            std::cout << "** SET_PARAMETER: missing wfd_client_rtp_ports" << std::endl;
            // TODO what is the correct error?
        } else {
            // TODO check port values
        }

        prop = props.find (WFD::PropertyName::name[WFD::PropertyType::WFD_PRESENTATION_URL]);
        if (prop == props.end()) {
            reply.set_response_code (400);
            std::cout << "** SET_PARAMETER: missing wfd_presentation_url" << std::endl;
            // TODO what is the correct error?
        } else {
            auto url = std::static_pointer_cast<WFD::PresentationUrl>((*prop).second);
            set_presentation_url (url->presentation_url_1());
        }

        // TODO require WFD_AV_FORMAT_CHANGE_TIMING in some cases
    }

    if (reply.response_code () == 200) {
        for (auto it = props.begin(); it != props.end(); it++) {
            if ((*it).first == WFD::PropertyName::name[WFD::PropertyType::WFD_AUDIO_CODECS]) {
            } else {
                /* Do this for known but unexpected properties
                    reply.set_response_code (303);
                    std::shared_ptr<WFD::PropertyErrors> error;
                    std::vector<unsigned short> v {451};
                    error.reset (new WFD::PropertyErrors((*it).first, v));
                    reply.payload().add_property_error(error);
                */  
                ;
            }
        }
    }

    if (reply.response_code() == 200)
        set_state(RTSP_SESSION_ESTABLISHMENT);

    send (reply);
}

void MiracSink::handle_m5_trigger_setup (std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    send (reply);

    // Send M6 SETUP

    expected_reply_ = WFD::Method::SETUP;
    WFD::Setup m6(presentation_url_);
    m6.header().set_cseq (send_cseq_++);
    send (m6);
}

void MiracSink::handle_m5_trigger_teardown (std::shared_ptr<WFD::Message> message)
{
    WFD::Reply reply(200);
    reply.header().set_cseq (message->header().cseq());

    send(reply);

    // Send M8 TEARDOWN
    expected_reply_ = WFD::Method::TEARDOWN;
    WFD::Teardown m8(presentation_url_);
    m8.header().set_cseq (send_cseq_++);
    send (m8);
}

void MiracSink::handle_m6_setup_reply (std::shared_ptr<WFD::Reply> reply)
{
    // not expecting anything
    expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

    // Ensure M6 SETUP reply is valid
    if (reply->response_code() != 200)
        return;

    if (reply->header().session().empty()) {
        std::cout << "** M6 reply without a session id, dropping" << std::endl;
        return;
    }
    set_session (reply->header().session());

    set_state (WFD_SESSION_ESTABLISHMENT);

    // Send M7
    expected_reply_ = WFD::Method::PLAY;
    WFD::Play m7(presentation_url_);
    m7.header().set_session (session_);
    m7.header().set_cseq (send_cseq_++);
    send (m7);
}

void MiracSink::handle_m7_play_reply (std::shared_ptr<WFD::Reply> reply)
{
    // not expecting anything
    expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

    // Ensure M7 PLAY reply is valid
    if (reply->response_code() != 200)
        return;

    set_state (WFD_SESSION);

    // TODO start playing stream...
}

void MiracSink::handle_m8_teardown_reply (std::shared_ptr<WFD::Reply> reply)
{
    // not expecting anything
    expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

    // Ensure M8 TEARDOWN reply is valid
    if (reply->response_code() != 200)
        return;

    set_state (INIT);
    // TODO
}

bool MiracSink::validate_message_sequence(std::shared_ptr<WFD::Message> message) const
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

void MiracSink::got_message(std::shared_ptr<WFD::Message> message)
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
            if (state_ == INIT)
                handle_m1_options(message);
            else 
                std::cout << "** Unexpected OPTIONS" << std::endl;
            break;
        case WFD::Message::MessageTypeGetParameter:
            if (state_ >= CAPABILITY_NEGOTIATION)
                handle_m3_get_parameter(message);
            else 
                std::cout << "** Unexpected GET_PARAMETER" << std::endl;
            break;
        case WFD::Message::MessageTypeSetParameter:
        {
            auto set_param = std::static_pointer_cast<WFD::SetParameter>(message);
            if (state_ == CAPABILITY_NEGOTIATION &&
                get_method(set_param) == SetParameterType::M4) {
                handle_m4_set_parameter (message, true);
            } else if (state_ >= RTSP_SESSION_ESTABLISHMENT &&
                get_method(set_param) == SetParameterType::M4) {
                handle_m4_set_parameter (message, false);
            } else if (state_ == RTSP_SESSION_ESTABLISHMENT &&
                       get_method(set_param) == SetParameterType::M5_TRIGGER_SETUP) {
                handle_m5_trigger_setup(message);
            } else if (state_ >= WFD_SESSION &&
                       get_method(set_param) == SetParameterType::M5_TRIGGER_TEARDOWN) {
                handle_m5_trigger_teardown(message);
            } else {
                std::cout << "** Unexpected SET_PARAMETER" << std::endl;
            }
            break;
        }
        case WFD::Message::MessageTypeReply:
            if (state_ == CAPABILITY_NEGOTIATION &&
                expected_reply_ == WFD::Method::OPTIONS) {
                handle_m2_options_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else if (state_ == RTSP_SESSION_ESTABLISHMENT &&
                       expected_reply_ == WFD::Method::SETUP) {
                handle_m6_setup_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else if (state_ == WFD_SESSION_ESTABLISHMENT &&
                       expected_reply_ == WFD::Method::PLAY) {
                handle_m7_play_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else if (state_ == WFD_SESSION &&
                       expected_reply_ == WFD::Method::TEARDOWN) {
                handle_m8_teardown_reply(std::static_pointer_cast<WFD::Reply>(message));
            } else {
                std::cout << "** Unexpected reply" << std::endl;
            }
            break;
        default:
            std::cout << "** Method not implemented" << std::endl;
            break;
    }
}

void MiracSink::on_connected()
{
    set_state(INIT);
    gst_pipeline.reset(new MiracGst(WFD_SINK, WFD_UNKNOWN_STREAM, "", 0));
}


MiracSink::~MiracSink()
{
}
