/*
 * This file is part of XXX
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * Contact: Jussi Kukkonen <jussi.kukkonen@intel.com>
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


#ifndef MIRAC_SINK_HPP
#define MIRAC_SINK_HPP

#include <memory>

#include "mirac-broker.hpp"
#include "reply.h"
#include "setparameter.h"
#include "mirac-gst.hpp"

class MiracSink: public MiracBroker
{
    public:
        MiracSink(const std::string& host, int rtsp_port);
        ~MiracSink();

    private:
        enum State {
            INIT,
            CAPABILITY_NEGOTIATION,
            RTSP_SESSION_ESTABLISHMENT,
            WFD_SESSION_ESTABLISHMENT, // RSTP SESSION OK
            WFD_SESSION, // WFD SESSION OK
        };

        enum SetParameterType {
            M4,
            M5_TRIGGER_SETUP,
            M5_TRIGGER_PLAY,
            M5_TRIGGER_PAUSE,
            M5_TRIGGER_TEARDOWN,
        };

        static SetParameterType get_method(std::shared_ptr<WFD::SetParameter> set_param);

        void got_message(std::shared_ptr<WFD::Message> message);
        void on_connected();

        bool validate_message_sequence(std::shared_ptr<WFD::Message> message) const;

        void handle_m1_options (std::shared_ptr<WFD::Message> message);
        void handle_m2_options_reply (std::shared_ptr<WFD::Reply> reply);
        void handle_m3_get_parameter (std::shared_ptr<WFD::Message> message);
        void handle_m4_set_parameter (std::shared_ptr<WFD::Message> message, bool initial);
        void handle_m5_trigger_setup (std::shared_ptr<WFD::Message> message);
        void handle_m5_trigger_teardown (std::shared_ptr<WFD::Message> message);
        void handle_m6_setup_reply (std::shared_ptr<WFD::Reply> reply);
        void handle_m7_play_reply (std::shared_ptr<WFD::Reply> reply);
        void handle_m8_teardown_reply (std::shared_ptr<WFD::Reply> reply);

        void set_state(MiracSink::State state);
        void set_presentation_url (std::string url);
        void set_session (std::string session);

        MiracSink::State state_;
        std::string presentation_url_;
        std::string session_;

        WFD::Method expected_reply_ = WFD::Method::ORG_WFA_WFD_1_0;

        int send_cseq_;
        int receive_cseq_;

        std::unique_ptr<MiracGst> gst_pipeline;
};

#endif  /* MIRAC_SINK_HPP */

