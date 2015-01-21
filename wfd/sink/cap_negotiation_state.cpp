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
#include "wfd/parser/propertyerrors.h"
#include "wfd/parser/reply.h"
#include "wfd/parser/setparameter.h"
#include "wfd/parser/standbyresumecapability.h"
#include "wfd/parser/triggermethod.h"
#include "wfd/parser/uibccapability.h"
#include "wfd/parser/videoformats.h"

namespace wfd {
namespace sink {


M3Handler::M3Handler(const InitParams& init_params)
  : MessageReceiver<Request::M3>(init_params) {
}

std::unique_ptr<Reply> M3Handler::HandleMessage(Message* message) {
  auto reply = std::unique_ptr<Reply>(new Reply(200));
  auto props = message->payload().get_parameter_properties();
  for (auto it = props.begin(); it != props.end(); ++it) {
      std::shared_ptr<wfd::Property> new_prop;
      if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_AUDIO_CODECS]){
          // declare that we support absolutely everything, let gstreamer deal with it
          auto codec_lpcm = new wfd::AudioCodec (wfd::AudioFormat::LPCM, wfd::AudioFormat::Modes(3), 0);
          auto codec_aac = new wfd::AudioCodec (wfd::AudioFormat::AAC, wfd::AudioFormat::Modes(15), 0);
          auto codec_ac3 = new wfd::AudioCodec (wfd::AudioFormat::AC3, wfd::AudioFormat::Modes(7), 0);
          auto codec_list = std::vector<wfd::AudioCodec>();
          codec_list.push_back(*codec_lpcm);
          codec_list.push_back(*codec_aac);
          codec_list.push_back(*codec_ac3);
          new_prop.reset(new wfd::AudioCodecs(codec_list));
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_VIDEO_FORMATS]){
          new_prop.reset(new wfd::VideoFormats(manager_->SupportedNativeVideoFormat(),
              false,
              manager_->SupportedH264VideoFormats()));
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_3D_FORMATS]){
          new_prop.reset(new wfd::Formats3d());
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_CONTENT_PROTECTION]){
          new_prop.reset(new wfd::ContentProtection());
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_DISPLAY_EDID]){
          new_prop.reset(new wfd::DisplayEdid());
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_COUPLED_SINK]){
          new_prop.reset(new wfd::CoupledSink());
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_CLIENT_RTP_PORTS]){
          new_prop.reset(new wfd::ClientRtpPorts(ToSinkMediaManager(manager_)->ListeningRtpPorts().first,
                                                 ToSinkMediaManager(manager_)->ListeningRtpPorts().second));
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_I2C]){
          new_prop.reset(new wfd::I2C(0));
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_UIBC_CAPABILITY]){
          new_prop.reset(new wfd::UIBCCapability());
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_CONNECTOR_TYPE]){
          new_prop.reset(new wfd::ConnectorType());
          reply->payload().add_property(new_prop);
      } else if (*it == wfd::PropertyName::name[wfd::PropertyType::WFD_STANDBY_RESUME_CAPABILITY]){
          new_prop.reset(new wfd::StandbyResumeCapability(false));
          reply->payload().add_property(new_prop);
      } else {
          std::cout << "** GET_PARAMETER: Property not supported" << std::endl;
      }
  }

  return std::move(reply);
}


M4Handler::M4Handler(const InitParams& init_params)
  : MessageReceiver<Request::M4>(init_params) {
}

std::unique_ptr<Reply> M4Handler::HandleMessage(Message* message) {
  auto presentation_url =
      static_cast<wfd::PresentationUrl*>(message->payload().get_property(wfd::WFD_PRESENTATION_URL).get());
  assert(presentation_url);
  SinkMediaManager* sink_media_manager= ToSinkMediaManager(manager_);
  sink_media_manager->SetPresentationUrl(presentation_url->presentation_url_1());

  auto video_formats =
      static_cast<wfd::VideoFormats*>(message->payload().get_property(wfd::WFD_VIDEO_FORMATS).get());
  assert(video_formats);
  if (!sink_media_manager->SetOptimalFormat(video_formats->GetSupportedH264Formats()[0])) {
    auto reply = std::unique_ptr<Reply>(new Reply(303));
    auto payload = std::unique_ptr<Payload>(new Payload());
    std::vector<unsigned short> error_codes = {415};
    auto property_errors =
        std::make_shared<PropertyErrors>(wfd::WFD_VIDEO_FORMATS, error_codes);
    payload->add_property_error(property_errors);
    reply->set_payload(std::move(payload));
    return std::move(reply);
  }

  return std::unique_ptr<Reply>(new Reply(200));
}

class M5Handler final : public MessageReceiver<Request::M5> {
 public:
  M5Handler(const InitParams& init_params)
    : MessageReceiver<Request::M5>(init_params) {
  }
  std::unique_ptr<Reply> HandleMessage(Message* message) override {
    auto property =
        static_cast<wfd::TriggerMethod*>(message->payload().get_property(wfd::WFD_TRIGGER_METHOD).get());

    auto reply = std::unique_ptr<Reply>(new Reply(200));
    reply->header().set_cseq(message->cseq());
    if (property->method() != wfd::TriggerMethod::SETUP) {
      reply->set_response_code(303);
    }

    return std::move(reply);
  }
};

CapNegotiationState::CapNegotiationState(const InitParams &init_params)
  : MessageSequenceWithOptionalSetHandler(init_params) {
  AddSequencedHandler(make_ptr(new M3Handler(init_params)));
  AddSequencedHandler(make_ptr(new M4Handler(init_params)));
  AddSequencedHandler(make_ptr(new M5Handler(init_params)));

  AddOptionalHandler(make_ptr(new M3Handler(init_params)));
  AddOptionalHandler(make_ptr(new M4Handler(init_params)));
}

}  // sink
}  // wfd
