%option debug
%option nodefault
%option c++
%option yyclass="wds::rtsp::ErrorScanner"
%option nounput nomain noyywrap
%option warn stack noyy_top_state
%option case-insensitive

%{
/*
 * This file is part of Wireless Display Software for Linux OS
 *
 * Copyright (C) 2015 Intel Corporation.
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


#include <string>
#include "errorscanner.h"
using token = wds::rtsp::Parser::token;
#define yyterminate() return(token::END)
%}

DIGIT [0-9]
DIGITS [0-9]+
SP [ \t]
CR [\r]
LF [\n]
CRLF {CR}{LF}

%%
  /* skip these */
[\r]
[\n]

 /* Convert these */
","  { return ','; }
":"  { return ':'; }

{SP}+ return token::WFD_SP;

^"wfd_audio_codecs" {
    return token::WFD_AUDIO_CODECS_ERROR;
  }

^"wfd_video_formats" {
    return token::WFD_VIDEO_FORMATS_ERROR;
  }

^"wfd_3d_video_formats" {
    return token::WFD_3D_FORMATS_ERROR;
  }

^"wfd_content_protection" {
    return token::WFD_CONTENT_PROTECTION_ERROR;
  }

^"wfd_display_edid" {
    return token::WFD_DISPLAY_EDID_ERROR;
  }

^"wfd_coupled_sink" {
    return token::WFD_COUPLED_SINK_ERROR;
  }

^"wfd_trigger_method" {
    return token::WFD_TRIGGER_METHOD_ERROR;
  }

^"wfd_presentation_url" {
    return token::WFD_PRESENTATION_URL_ERROR;
  }

^"wfd_client_rtp_ports" {
    return token::WFD_CLIENT_RTP_PORTS_ERROR;
  }

^"wfd_route" {
    return token::WFD_ROUTE_ERROR;
  }

^"wfd_I2C" {
    return token::WFD_I2C_ERROR;
  }

^"wfd_av_format_change_timing" {
    return token::WFD_AV_FORMAT_CHANGE_TIMING_ERROR;
  }

^"wfd_preferred_display_mode" {
    return token::WFD_PREFERRED_DISPLAY_MODE_ERROR;
  }

^"wfd_uibc_capability" {
    return token::WFD_UIBC_CAPABILITY_ERROR;
  }

^"wfd_uibc_setting" {
    return token::WFD_UIBC_SETTING_ERROR;
  }

^"wfd_standby_resume_capability" {
    return token::WFD_STANDBY_RESUME_CAPABILITY_ERROR;
  }

^"wfd_standby" {
    return token::WFD_STANDBY_ERROR;
  }

^"wfd_connector_type" {
    return token::WFD_CONNECTOR_TYPE_ERROR;
  }

^"wfd_idr_request" {
    return token::WFD_IDR_REQUEST_ERROR;
  }

^[[:alpha:]][[:alnum:]\-\_]* {
    yylval->sval = new std::string(yytext, yyleng);
    return token::WFD_GENERIC_PROPERTY_ERROR;
  }

{DIGITS} {
    std::string str(yytext, yyleng);
    str += '\0';
    yylval->nval = strtoull(str.c_str(), NULL, 10);
    if (errno == ERANGE) {
      // todo: handle error
    }
    return token::WFD_NUM;
  }

 /* all unmatched */
<*>. {}
%%
