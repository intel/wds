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

#include "libwds/common/rtsp_status_code.h"
#include "libwds/parser/audiocodecs.h"
#include "libwds/parser/clientrtpports.h"
#include "libwds/parser/connectortype.h"
#include "libwds/parser/contentprotection.h"
#include "libwds/parser/coupledsink.h"
#include "libwds/parser/displayedid.h"
#include "libwds/parser/formats3d.h"
#include "libwds/parser/i2c.h"
#include "libwds/public/media_manager.h"
#include "libwds/parser/getparameter.h"
#include "libwds/parser/payload.h"
#include "libwds/parser/presentationurl.h"
#include "libwds/parser/propertyerrors.h"
#include "libwds/parser/reply.h"
#include "libwds/parser/setparameter.h"
#include "libwds/parser/standbyresumecapability.h"
#include "libwds/parser/triggermethod.h"
#include "libwds/parser/uibccapability.h"
#include "libwds/parser/videoformats.h"

namespace wds {
namespace sink {


M3Handler::M3Handler(const InitParams& init_params)
  : MessageReceiver<Request::M3>(init_params) {
}

std::unique_ptr<Reply> M3Handler::HandleMessage(Message* message) {
  auto reply = std::unique_ptr<Reply>(new Reply(RTSP_OK));
  auto props = message->payload().get_parameter_properties();
  for (auto it = props.begin(); it != props.end(); ++it) {
      std::shared_ptr<Property> new_prop;
      if (*it == PropertyName::name[PropertyType::WFD_AUDIO_CODECS]){
          // FIXME: declare that we support absolutely every audio codec/format,
          // but there should be a MediaManager API for it
          auto codec_lpcm = AudioCodec(LPCM, AudioModes(3), 0);
          auto codec_aac = AudioCodec(AAC, AudioModes(15), 0);
          auto codec_ac3 = AudioCodec(AC3, AudioModes(7), 0);
          std::vector<AudioCodec> codec_list;
          codec_list.push_back(codec_lpcm);
          codec_list.push_back(codec_aac);
          codec_list.push_back(codec_ac3);
          new_prop.reset(new AudioCodecs(codec_list));
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_VIDEO_FORMATS]){
          new_prop.reset(new VideoFormats(ToSinkMediaManager(manager_)->GetSupportedNativeVideoFormat(),
              false,
              ToSinkMediaManager(manager_)->GetSupportedH264VideoFormats()));
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_3D_FORMATS]){
          new_prop.reset(new Formats3d());
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_CONTENT_PROTECTION]){
          new_prop.reset(new ContentProtection());
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_DISPLAY_EDID]){
          new_prop.reset(new DisplayEdid());
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_COUPLED_SINK]){
          new_prop.reset(new CoupledSink());
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_CLIENT_RTP_PORTS]){
          new_prop.reset(new ClientRtpPorts(ToSinkMediaManager(manager_)->GetLocalRtpPorts().first,
                                            ToSinkMediaManager(manager_)->GetLocalRtpPorts().second));
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_I2C]){
          new_prop.reset(new I2C(0));
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_UIBC_CAPABILITY]){
          new_prop.reset(new UIBCCapability());
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_CONNECTOR_TYPE]){
          new_prop.reset(new ConnectorType());
          reply->payload().add_property(new_prop);
      } else if (*it == PropertyName::name[PropertyType::WFD_STANDBY_RESUME_CAPABILITY]){
          new_prop.reset(new StandbyResumeCapability(false));
          reply->payload().add_property(new_prop);
      } else {
          WDS_ERROR("** GET_PARAMETER: Property not supported");
          return std::unique_ptr<Reply>(new Reply(RTSP_NotImplemented));
      }
  }

  return std::move(reply);
}


M4Handler::M4Handler(const InitParams& init_params)
  : MessageReceiver<Request::M4>(init_params) {
}

std::unique_ptr<Reply> M4Handler::HandleMessage(Message* message) {
  auto presentation_url =
      static_cast<PresentationUrl*>(message->payload().get_property(WFD_PRESENTATION_URL).get());
  assert(presentation_url);
  SinkMediaManager* sink_media_manager = ToSinkMediaManager(manager_);
  sink_media_manager->SetPresentationUrl(presentation_url->presentation_url_1());

  auto video_formats =
      static_cast<VideoFormats*>(message->payload().get_property(WFD_VIDEO_FORMATS).get());
  assert(video_formats);
  if (!sink_media_manager->SetOptimalVideoFormat(video_formats->GetSelectableH264Formats()[0])) {
    auto reply = std::unique_ptr<Reply>(new Reply(RTSP_SeeOther));
    auto payload = std::unique_ptr<Payload>(new Payload());
    std::vector<unsigned short> error_codes = {RTSP_UnsupportedMediaType};
    auto property_errors =
        std::make_shared<PropertyErrors>(WFD_VIDEO_FORMATS, error_codes);
    payload->add_property_error(property_errors);
    reply->set_payload(std::move(payload));
    return std::move(reply);
  }

  return std::unique_ptr<Reply>(new Reply(RTSP_OK));
}

class M5Handler final : public MessageReceiver<Request::M5> {
 public:
  M5Handler(const InitParams& init_params)
    : MessageReceiver<Request::M5>(init_params) {
  }
  std::unique_ptr<Reply> HandleMessage(Message* message) override {
    auto property =
        static_cast<TriggerMethod*>(message->payload().get_property(WFD_TRIGGER_METHOD).get());

    auto reply = std::unique_ptr<Reply>(new Reply(RTSP_OK));
    reply->header().set_cseq(message->cseq());
    if (property->method() != TriggerMethod::SETUP) {
      reply->set_response_code(RTSP_SeeOther);
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
