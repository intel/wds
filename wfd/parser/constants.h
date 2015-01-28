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


#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace wfd {

const char SEMICOLON[] = ":";
const char SPACE[] = " ";
const char NONE[] = "none";
const char CRLF[] = "\r\n";
const char RTSP_END[] = "RTSP/1.0";

enum PropertyType {
  WFD_AUDIO_CODECS,
  WFD_VIDEO_FORMATS,
  WFD_3D_FORMATS,
  WFD_CONTENT_PROTECTION,
  WFD_DISPLAY_EDID,
  WFD_COUPLED_SINK,
  WFD_TRIGGER_METHOD,
  WFD_PRESENTATION_URL,
  WFD_CLIENT_RTP_PORTS,
  WFD_ROUTE,
  WFD_I2C,
  WFD_AV_FORMAT_CHANGE_TIMING,
  WFD_PREFERRED_DISPLAY_MODE,
  WFD_UIBC_CAPABILITY,
  WFD_UIBC_SETTING,
  WFD_STANDBY_RESUME_CAPABILITY,
  WFD_STANDBY,
  WFD_CONNECTOR_TYPE,
  WFD_IDR_REQUEST,
  WFD_GENERIC
};

namespace PropertyName {
  const char wfd_audio_codecs[] = "wfd_audio_codecs";
  const char wfd_video_formats[] = "wfd_video_formats";
  const char wfd_3d_formats[] = "wfd_3d_video_formats";
  const char wfd_content_protection[] = "wfd_content_protection";
  const char wfd_display_edid[] = "wfd_display_edid";
  const char wfd_coupled_sink[] = "wfd_coupled_sink";
  const char wfd_trigger_method[] = "wfd_trigger_method";
  const char wfd_presentation_url[] = "wfd_presentation_URL";
  const char wfd_client_rtp_ports[] = "wfd_client_rtp_ports";
  const char wfd_route[] = "wfd_route";
  const char wfd_I2C[] = "wfd_I2C";
  const char wfd_av_format_change_timing[] = "wfd_av_format_change_timing";
  const char wfd_preferred_display_mode[] = "wfd_preferred_display_mode";
  const char wfd_uibc_capability[] = "wfd_uibc_capability";
  const char wfd_uibc_setting[] = "wfd_uibc_setting";
  const char wfd_standby_resume_capability[] = "wfd_standby_resume_capability";
  const char wfd_standby[] = "wfd_standby";
  const char wfd_connector_type[] = "wfd_connector_type";
  const char wfd_idr_request[] = "wfd_idr_request";

  const char* const name[] =
    {
      wfd_audio_codecs,
      wfd_video_formats,
      wfd_3d_formats,
      wfd_content_protection,
      wfd_display_edid,
      wfd_coupled_sink,
      wfd_trigger_method,
      wfd_presentation_url,
      wfd_client_rtp_ports,
      wfd_route,
      wfd_I2C,
      wfd_av_format_change_timing,
      wfd_preferred_display_mode,
      wfd_uibc_capability,
      wfd_uibc_setting,
      wfd_standby_resume_capability,
      wfd_standby,
      wfd_connector_type,
      wfd_idr_request
    };
}

enum Method {
  OPTIONS,
  SET_PARAMETER,
  GET_PARAMETER,
  SETUP,
  PLAY,
  TEARDOWN,
  PAUSE,
  ORG_WFA_WFD_1_0
};

namespace MethodName {
  const char OPTIONS[] = "OPTIONS";
  const char SET_PARAMETER[] = "SET_PARAMETER";
  const char GET_PARAMETER[] = "GET_PARAMETER";
  const char SETUP[] = "SETUP";
  const char PLAY[] = "PLAY";
  const char TEARDOWN[] = "TEARDOWN";
  const char PAUSE[] = "PAUSE";
  const char ORG_WFA_WFD1_0[] = "org.wfa.wfd1.0";

  const char* const name[] = { OPTIONS,
                              SET_PARAMETER,
                              GET_PARAMETER,
                              SETUP,
                              PLAY,
                              TEARDOWN,
                              PAUSE,
                              ORG_WFA_WFD1_0 };
}

} // namespace wfd

#endif // CONSTANTS_H_
