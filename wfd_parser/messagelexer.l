%option debug
%option nodefault
%option c++
%option yyclass="WFD::MessageScanner"
%option outfile="messagescanner.cpp"
%option prefix="Msg_"
%option nounput nomain noyywrap
%option warn stack noyy_top_state
%option case-insensitive

%{
/*
 * This file is part of WiFi Display RTSP parser (wysiwidi).
 * Copyright (c) 2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */



#include <string>
#include "messagescanner.h"
typedef WFD::Parser::token token; 
#define yyterminate() return(token::END)
%}

%x MATCH_STRING_STATE
%x MATCH_RESPONSE_CODE_STATE

%s SUPPORTED_METHODS_STATE
%s CONTENT_TYPE_STATE
%s SESSION_STATE
%s MATCH_PRESENTATION_URL
%s NUM_AS_HEX_MODE

DIGIT [0-9]
DIGITS [0-9]+
HEXDIG [0-9a-fA-F]
HEXDIGITS [0-9a-fA-F]+
SP [ \t]
CR [\r]
LF [\n]
CRLF {CR}{LF}
IPADDRESS (([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]) 
IPPORT ({DIGIT}){1,5}
ERROR ({DIGIT}){3}

%%
  /* skip these */
<*>[\r] { BEGIN(INITIAL); }
<*>[\n] { BEGIN(INITIAL); }

 /* Convert these */
"="  { return '='; }
"-"  { return '-'; }
","  { return ','; }
"\*" { return '*'; }
";"  { return ';'; }
":"  { return ':'; }
"/"  { return '/'; }

{SP}+ return token::WFD_SP;

"none" return token::WFD_NONE;
"LPCM" return token::WFD_AUDIO_CODEC_LPCM;
"AAC" return token::WFD_AUDIO_CODEC_AAC;
"AC3" return token::WFD_AUDIO_CODEC_AC3;
"HDCP2.0" return token::WFD_HDCP_SPEC_2_0;
"HDCP2.1" return token::WFD_HDCP_SPEC_2_1;
"port=" return token::WFD_IP_PORT;
"RTP/AVP/UDP;unicast" return token::WFD_STREAM_PROFILE;
"mode=play" return token::WFD_MODE_PLAY;
"primary" return token::WFD_ROUTE_PRIMARY;
"secondary" return token::WFD_ROUTE_SECONDARY;
"input_category_list=" return token::WFD_INPUT_CATEGORY_LIST;
"GENERIC" return token::WFD_INPUT_CATEGORY_GENERIC;
"HIDC" return token::WFD_INPUT_CATEGORY_HIDC;
"generic_cap_list=" return token::WFD_GENERIC_CAP_LIST;
"Keyboard" return token::WFD_INPUT_TYPE_KEYBOARD;
"Mouse" return token::WFD_INPUT_TYPE_MOUSE;
"SingleTouch" return token::WFD_INPUT_TYPE_SINGLE_TOUCH;
"MultiTouch" return token::WFD_INPUT_TYPE_MULTI_TOUCH;
"Joystick" return token::WFD_INPUT_TYPE_JOYSTICK;
"Camera" return token::WFD_INPUT_TYPE_CAMERA;
"Gesture" return token::WFD_INPUT_TYPE_GESTURE;
"RemoteControl" return token::WFD_INPUT_TYPE_REMOTE_CONTROL;
"hidc_cap_list=" return token::WFD_HIDC_CAP_LIST;
"Infrared" return token::WFD_INPUT_PATH_INFRARED;
"USB" return token::WFD_INPUT_PATH_USB;
"BT" return token::WFD_INPUT_PATH_BT;
"Wi-Fi" return token::WFD_INPUT_PATH_WIFI;
"Zigbee" return token::WFD_INPUT_PATH_ZIGBEE;
"No-SP" return token::WFD_INPUT_PATH_NOSP;
"disable" return token::WFD_UIBC_SETTING_DISABLE;
"enable" return token::WFD_UIBC_SETTING_ENABLE;
"supported" return token::WFD_SUPPORTED;

<NUM_AS_HEX_MODE>{DIGITS} {
    if (yyleng > 16) {
      yylval->sval = new std::string(yytext);
      return token::WFD_STRING;
    }

    std::string str(yytext, yyleng);
    str += '\0';
    yylval->nval = strtoull(str.c_str(), NULL, 16);
    if (errno == ERANGE) {
      // todo: handle error
    }
    return token::WFD_NUM;
  }

<NUM_AS_HEX_MODE>{HEXDIGITS} {
    if (yyleng > 16) {
      yylval->sval = new std::string(yytext);
      return token::WFD_STRING;
    }

    std::string str(yytext, yyleng);
    str += '\0';
    yylval->nval = strtoull(str.c_str(), NULL, 16);
    if (errno == ERANGE) {
      // todo: handle error
    }
    return token::WFD_NUM;
  }

^"wfd_audio_codecs" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_AUDIO_CODECS;
  }

^"wfd_video_formats" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_VIDEO_FORMATS;
  }

^"wfd_3d_video_formats" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_3D_FORMATS;
  }

^"wfd_content_protection" {
    return token::WFD_CONTENT_PROTECTION;
  }

^"wfd_display_edid" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_DISPLAY_EDID;
  }

^"wfd_coupled_sink" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_COUPLED_SINK;
  }

^"wfd_trigger_method" {
    BEGIN(SUPPORTED_METHODS_STATE);
    return token::WFD_TRIGGER_METHOD;
  }

^"wfd_presentation_url" {
    BEGIN(MATCH_PRESENTATION_URL);
    return token::WFD_PRESENTATION_URL;
  }

^"wfd_client_rtp_ports" {
    return token::WFD_CLIENT_RTP_PORTS;
  }

^"wfd_route" {
    return token::WFD_ROUTE;
  }

^"wfd_I2C" {
    return token::WFD_I2C;
  }

^"wfd_av_format_change_timing" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_AV_FORMAT_CHANGE_TIMING;
  }

^"wfd_preferred_display_mode" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_PREFERRED_DISPLAY_MODE;
  }

^"wfd_uibc_capability" {
    return token::WFD_UIBC_CAPABILITY;
  }

^"wfd_uibc_setting" {
    return token::WFD_UIBC_SETTING;
  }

^"wfd_standby_resume_capability" {
    return token::WFD_STANDBY_RESUME_CAPABILITY;
  }

^"wfd_standby" {
    if (is_reply_message_)
      return token::WFD_STANDBY_IN_RESPONSE;
    return token::WFD_STANDBY_IN_REQUEST;
  }

^"wfd_connector_type" {
    BEGIN(NUM_AS_HEX_MODE);
    return token::WFD_CONNECTOR_TYPE;
  }

^"wfd_idr_request" {
    if (is_reply_message_)
      return token::WFD_IDR_REQUEST_IN_RESPONSE;
    return token::WFD_IDR_REQUEST_IN_REQUEST;
  }

^[[:alpha:]][[:alnum:]\-\_]* {
    BEGIN(MATCH_STRING_STATE);
    yylval->sval = new std::string(yytext, yyleng);
    return token::WFD_GENERIC_PROPERTY;
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

{HEXDIGITS} {
    std::string str(yytext, yyleng);
    str += '\0';
    yylval->nval = strtoull(str.c_str(), NULL, 16);
    if (errno == ERANGE) {
      // todo: handle error
    }
    return token::WFD_NUM;
  }

<MATCH_STRING_STATE>[^ :\r\n][^\r\n]+/"\r\n" {
    BEGIN(INITIAL);
    yylval->sval = new std::string(yytext);
    return token::WFD_STRING;
  }

<SUPPORTED_METHODS_STATE>"OPTIONS" {
    return token::WFD_OPTIONS;
  }

<SUPPORTED_METHODS_STATE>"SET_PARAMETER" {
    return token::WFD_SET_PARAMETER;
  }

<SUPPORTED_METHODS_STATE>"GET_PARAMETER" {
    return token::WFD_GET_PARAMETER;
  }

<SUPPORTED_METHODS_STATE>"SETUP" {
    return token::WFD_SETUP;
  }

<SUPPORTED_METHODS_STATE>"PLAY" {
    return token::WFD_PLAY;
  }

<SUPPORTED_METHODS_STATE>"TEARDOWN" {
    return token::WFD_TEARDOWN;
  }

<SUPPORTED_METHODS_STATE>"PAUSE" {
    return token::WFD_PAUSE;
  }
     
<SUPPORTED_METHODS_STATE>"org.wfa.wfd1.0" {
  return token::WFD_TAG;
  }

<MATCH_PRESENTATION_URL>"rtsp://"[^ \t\n]+{IPADDRESS}"/wfd1.0/streamid=0" {
    yylval->sval = new std::string(yytext);
    return token::WFD_PRESENTATION_URL_0;
  }

<MATCH_PRESENTATION_URL>"rtsp://"[^ \t\n]+{IPADDRESS}"/wfd1.0/streamid=1" {
    yylval->sval = new std::string(yytext);
    return token::WFD_PRESENTATION_URL_1;
  }

 /* all unmatched */
<*>. {}
%%
