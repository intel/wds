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

#include "cap_negotiation_state.h"

#include <iostream>

#include "wfd/parser/audiocodecs.h"
#include "wfd/parser/clientrtpports.h"
#include "wfd/parser/connectortype.h"
#include "wfd/parser/contentprotection.h"
#include "wfd/parser/coupledsink.h"
#include "wfd/parser/displayedid.h"
#include "wfd/parser/formats3d.h"
#include "wfd/parser/i2c.h"
#include "wfd/public/media_manager.h"
#include "wfd/parser/getparameter.h"
#include "wfd/parser/payload.h"
#include "wfd/parser/presentationurl.h"
#include "wfd/parser/reply.h"
#include "wfd/parser/setparameter.h"
#include "wfd/parser/standbyresumecapability.h"
#include "wfd/parser/triggermethod.h"
#include "wfd/common/typed_message.h"
#include "wfd/parser/uibccapability.h"
#include "wfd/parser/videoformats.h"

namespace wfd {


M3Handler::M3Handler(const InitParams& init_params)
  : MessageReceiver<TypedMessage::M3>(init_params) {
}

std::unique_ptr<WFD::Reply> M3Handler::HandleMessage(TypedMessage* message) {
  auto reply = std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
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
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_VIDEO_FORMATS]){
          auto codec_list = WFD::H264Codecs();
          // again, declare that we support absolutely everything, let gstreamer deal with it
          auto codec_cbp = new WFD::H264Codec(1, 16, 0x1ffff, 0x1fffffff, 0xfff, 0, 0, 0, 0x11, 0, 0);
          auto codec_chp = new WFD::H264Codec(2, 16, 0x1ffff, 0x1fffffff, 0xfff, 0, 0, 0, 0x11, 0, 0);
          codec_list.push_back(*codec_cbp);
          codec_list.push_back(*codec_chp);
          new_prop.reset(new WFD::VideoFormats(64 , 0, codec_list)); // 64 should mean 1920x1080p24
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_3D_FORMATS]){
          new_prop.reset(new WFD::Formats3d());
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_CONTENT_PROTECTION]){
          new_prop.reset(new WFD::ContentProtection());
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_DISPLAY_EDID]){
          new_prop.reset(new WFD::DisplayEdid());
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_COUPLED_SINK]){
          new_prop.reset(new WFD::CoupledSink());
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_CLIENT_RTP_PORTS]){
          new_prop.reset(new WFD::ClientRtpPorts(manager_->RtpPort(), 0));
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_I2C]){
          new_prop.reset(new WFD::I2C(0));
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_UIBC_CAPABILITY]){
          new_prop.reset(new WFD::UIBCCapability());
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_CONNECTOR_TYPE]){
          new_prop.reset(new WFD::ConnectorType());
          reply->payload().add_property(new_prop);
      } else if (*it == WFD::PropertyName::name[WFD::PropertyType::WFD_STANDBY_RESUME_CAPABILITY]){
          new_prop.reset(new WFD::StandbyResumeCapability(false));
          reply->payload().add_property(new_prop);
      } else {
          std::cout << "** GET_PARAMETER: Property not supported" << std::endl;
      }
  }

  return std::move(reply);
}


M4Handler::M4Handler(const InitParams& init_params)
  : MessageReceiver<TypedMessage::M4>(init_params) {
}

std::unique_ptr<WFD::Reply> M4Handler::HandleMessage(TypedMessage* message) {
  auto property =
      static_cast<WFD::PresentationUrl*>(message->payload().get_property(WFD::WFD_PRESENTATION_URL).get());

  // presentation URL is the only thing we care about
  // support for other parameters can be added later as needed
  manager_->SetPresentationUrl(property->presentation_url_1());
  return std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
}

class M5Handler final : public MessageReceiver<TypedMessage::M5> {
 public:
  M5Handler(const InitParams& init_params)
    : MessageReceiver<TypedMessage::M5>(init_params) {
  }
  virtual std::unique_ptr<WFD::Reply> HandleMessage(TypedMessage* message) override {
    auto property =
        static_cast<WFD::TriggerMethod*>(message->payload().get_property(WFD::WFD_TRIGGER_METHOD).get());

    auto reply = std::unique_ptr<WFD::Reply>(new WFD::Reply(200));
    reply->header().set_cseq(message->cseq());
    if (property->method() != WFD::TriggerMethod::SETUP) {
      reply->set_response_code(303);
    }

    return std::move(reply);
  }
};

CapNegotiationState::CapNegotiationState(const InitParams &init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(new M3Handler(init_params));
  AddSequencedHandler(new M4Handler(init_params));
  AddSequencedHandler(new M5Handler(init_params));

  AddOptionalHandler(new M3Handler(init_params));
  AddOptionalHandler(new M4Handler(init_params));
}

CapNegotiationState::~CapNegotiationState() {
}

}  // miracast
