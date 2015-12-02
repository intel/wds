/*
 * This file is part of Wireless Display Software for Linux OS
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

#include "libwds/rtsp/audiocodecs.h"
#include "libwds/rtsp/clientrtpports.h"
#include "libwds/rtsp/connectortype.h"
#include "libwds/rtsp/contentprotection.h"
#include "libwds/rtsp/coupledsink.h"
#include "libwds/rtsp/displayedid.h"
#include "libwds/rtsp/formats3d.h"
#include "libwds/rtsp/i2c.h"
#include "libwds/public/media_manager.h"
#include "libwds/rtsp/getparameter.h"
#include "libwds/rtsp/payload.h"
#include "libwds/rtsp/presentationurl.h"
#include "libwds/rtsp/propertyerrors.h"
#include "libwds/rtsp/reply.h"
#include "libwds/rtsp/setparameter.h"
#include "libwds/rtsp/standbyresumecapability.h"
#include "libwds/rtsp/triggermethod.h"
#include "libwds/rtsp/uibccapability.h"
#include "libwds/rtsp/videoformats.h"

namespace wds {
using rtsp::Message;
using rtsp::Request;
using rtsp::Reply;

namespace sink {


M3Handler::M3Handler(const InitParams& init_params)
  : MessageReceiver<Request::M3>(init_params) {
}

std::unique_ptr<Reply> M3Handler::HandleMessage(Message* message) {
  // FIXME : resolve clashes between wds exported and internal rtsp type names.
  using namespace rtsp;

  auto reply = std::unique_ptr<Reply>(new Reply(rtsp::STATUS_OK));
  auto props = message->payload().get_parameter_properties();
  for (auto it = props.begin(); it != props.end(); ++it) {
      std::shared_ptr<rtsp::Property> new_prop;
      if (*it == GetPropertyName(rtsp::AudioCodecsPropertyType)){
          // FIXME: declare that we support absolutely every audio codec/format,
          // but there should be a MediaManager API for it
          auto codec_lpcm = AudioCodec(LPCM, AudioModes(3), 0);
          auto codec_aac = AudioCodec(AAC, AudioModes(15), 0);
          auto codec_ac3 = AudioCodec(AC3, AudioModes(7), 0);
          std::vector<AudioCodec> codec_list;
          codec_list.push_back(codec_lpcm);
          codec_list.push_back(codec_aac);
          codec_list.push_back(codec_ac3);
          new_prop.reset(new rtsp::AudioCodecs(codec_list));
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::VideoFormatsPropertyType)){
          new_prop.reset(new rtsp::VideoFormats(ToSinkMediaManager(manager_)->GetNativeVideoFormat(),
              false,
              ToSinkMediaManager(manager_)->GetSupportedH264VideoCodecs()));
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::Video3DFormatsPropertyType)){
          new_prop.reset(new Formats3d());
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::ContentProtectionPropertyType)){
          new_prop.reset(new ContentProtection());
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::DisplayEdidPropertyType)){
          new_prop.reset(new DisplayEdid());
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::CoupledSinkPropertyType)){
          new_prop.reset(new CoupledSink());
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::ClientRTPPortsPropertyType)){
          new_prop.reset(new ClientRtpPorts(ToSinkMediaManager(manager_)->GetLocalRtpPorts().first,
                                            ToSinkMediaManager(manager_)->GetLocalRtpPorts().second));
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::I2CPropertyType)){
          new_prop.reset(new I2C(0));
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::UIBCCapabilityPropertyType)){
          new_prop.reset(new UIBCCapability());
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::ConnectorTypePropertyType)){
          new_prop.reset(new rtsp::ConnectorType(ToSinkMediaManager(manager_)->GetConnectorType()));
          reply->payload().add_property(new_prop);
      } else if (*it == GetPropertyName(rtsp::StandbyResumeCapabilityPropertyType)){
          new_prop.reset(new StandbyResumeCapability(false));
          reply->payload().add_property(new_prop);
      } else {
          WDS_WARNING("** GET_PARAMETER: Ignoring unsupported property '%s'.", (*it).c_str());
      }
  }

  return std::move(reply);
}


M4Handler::M4Handler(const InitParams& init_params)
  : MessageReceiver<Request::M4>(init_params) {
}

std::unique_ptr<Reply> M4Handler::HandleMessage(Message* message) {
  SinkMediaManager* sink_media_manager = ToSinkMediaManager(manager_);

  auto presentation_url =
      static_cast<rtsp::PresentationUrl*>(message->payload().get_property(rtsp::PresentationURLPropertyType).get());
  if (presentation_url) {
    sink_media_manager->SetPresentationUrl(presentation_url->presentation_url_1());
  }

  auto video_formats =
      static_cast<rtsp::VideoFormats*>(message->payload().get_property(rtsp::VideoFormatsPropertyType).get());

  if (!video_formats) {
    WDS_ERROR("Failed to obtain 'wfd-video-formats' in M4 handler.");
    return nullptr;
  }

  const auto& selected_formats = video_formats->GetH264Formats();
  if (selected_formats.size() != 1) {
    WDS_ERROR("Failed to obtain optimal video format from 'wfd-video-formats' in M4 handler.");
    return nullptr;
  }

  if (!sink_media_manager->SetOptimalVideoFormat(selected_formats[0])) {
    auto reply = std::unique_ptr<Reply>(new Reply(rtsp::STATUS_SeeOther));
    auto payload = std::unique_ptr<rtsp::Payload>(new rtsp::Payload());
    std::vector<unsigned short> error_codes = {rtsp::STATUS_UnsupportedMediaType};
    auto property_errors =
        std::make_shared<rtsp::PropertyErrors>(rtsp::VideoFormatsPropertyType, error_codes);
    payload->add_property_error(property_errors);
    reply->set_payload(std::move(payload));
    return std::move(reply);
  }

  return std::unique_ptr<Reply>(new Reply(rtsp::STATUS_OK));
}

class M5Handler final : public MessageReceiver<Request::M5> {
 public:
  M5Handler(const InitParams& init_params)
    : MessageReceiver<Request::M5>(init_params) {
  }
  std::unique_ptr<Reply> HandleMessage(Message* message) override {
    auto property =
        static_cast<rtsp::TriggerMethod*>(message->payload().get_property(rtsp::TriggerMethodPropertyType).get());

    auto reply = std::unique_ptr<Reply>(new Reply(rtsp::STATUS_OK));
    reply->header().set_cseq(message->cseq());
    if (property->method() != rtsp::TriggerMethod::SETUP) {
      reply->set_response_code(rtsp::STATUS_SeeOther);
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
}  // wds
