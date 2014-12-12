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

#include "wfd/parser/clientrtpports.h"
#include "wfd/public/media_manager.h"
#include "wfd/parser/getparameter.h"
#include "wfd/parser/payload.h"
#include "wfd/parser/presentationurl.h"
#include "wfd/parser/reply.h"
#include "wfd/parser/setparameter.h"
#include "wfd/common/typed_message.h"

namespace wfd {

class M3Handler final : public SequencedMessageSender {
 public:
  using SequencedMessageSender::SequencedMessageSender;

 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override;
  virtual bool HandleReply(Reply* reply) override;
};

class M4Handler final : public SequencedMessageSender {
 public:
  using SequencedMessageSender::SequencedMessageSender;

 private:
  virtual std::unique_ptr<TypedMessage> CreateMessage() override;
  virtual bool HandleReply(Reply* reply) override;
};

std::unique_ptr<TypedMessage> M3Handler::CreateMessage() {
  auto get_param =
      std::make_shared<WFD::GetParameter>("rtsp://localhost/wfd1.0");
  get_param->header().set_cseq(send_cseq_++);
  // todo: get data from environment (e.g. if video / audio / hdcp required)
  // and request only needed parameters
  std::vector<std::string> props;
  props.push_back("wfd_video_formats");
  props.push_back("wfd_audio_codecs");
  props.push_back("wfd_client_rtp_ports");
  get_param->set_payload(new WFD::Payload(props));
  return std::unique_ptr<M3>(new M3(get_param));
}

bool M3Handler::HandleReply(Reply* reply) {
  if (reply->GetResponseCode() != 200)
    return false;

  auto payload = reply->message()->payload();
  auto prop = payload.get_property(WFD::WFD_CLIENT_RTP_PORTS);
  auto ports = static_cast<WFD::ClientRtpPorts*>(prop.get());
  assert(ports);
  manager_->SetRtpPorts(ports->rtp_port_0(), ports->rtp_port_1());
  return true;
}

std::unique_ptr<TypedMessage> M4Handler::CreateMessage() {
  auto set_param =
      std::make_shared<WFD::SetParameter>("rtsp://localhost/wfd1.0");
  set_param->header().set_cseq(send_cseq_++);
  set_param->payload().add_property(
      std::shared_ptr<WFD::Property>(new WFD::ClientRtpPorts(1028,0)));
  set_param->payload().add_property(
      std::shared_ptr<WFD::Property>(new WFD::PresentationUrl(
          "rtsp://192.168.173.1/wfd1.0/streamid=0",
          "")));
  return std::unique_ptr<M4>(new M4(set_param));
}

bool M4Handler::HandleReply(Reply* reply) {
  return (reply->GetResponseCode() == 200);
}

CapNegotiationState::CapNegotiationState(const InitParams &init_params)
  : MessageSequenceHandler(init_params) {
  AddSequencedHandler(new M3Handler(init_params));
  AddSequencedHandler(new M4Handler(init_params));
}

CapNegotiationState::~CapNegotiationState() {
}

}  // miracast
