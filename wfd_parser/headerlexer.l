%option debug
%option nodefault
%option c++
%option yyclass="WFD::HeaderScanner"
%option outfile="headerscanner.cpp"
%option prefix="Hdr_"
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
#include "headerscanner.h"
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

";timeout=" return token::WFD_TIMEOUT;
";server_port=" return token::WFD_SERVER_PORT;

^(?-i:"OPTIONS") {
    BEGIN(INITIAL);
    return token::WFD_OPTIONS;
  }

^(?-i:"SET_PARAMETER") {
    BEGIN(INITIAL);
    return token::WFD_SET_PARAMETER;
  }

^(?-i:"GET_PARAMETER") {
    BEGIN(INITIAL);
    return token::WFD_GET_PARAMETER;
  }

^(?-i:"SETUP") {
    BEGIN(INITIAL);
    return token::WFD_SETUP;
  }

^(?-i:"PLAY") {
    BEGIN(INITIAL);
    return token::WFD_PLAY;
  }

^(?-i:"TEARDOWN") {
    BEGIN(INITIAL);
    return token::WFD_TEARDOWN;
  }

^(?-i:"PAUSE") {
    BEGIN(INITIAL);
    return token::WFD_PAUSE;
  }

  /* RTSP response, get reply code, RTSP/1.0 200 OK */
^"RTSP/"{DIGIT}"."{DIGIT}{SP}+ {
    BEGIN(MATCH_RESPONSE_CODE_STATE);
    return token::WFD_RESPONSE;
  }

  /* CSeq: i */
^"CSeq:" {
    BEGIN(INITIAL);
    return token::WFD_CSEQ;
  }

^"Public:" {
    BEGIN(SUPPORTED_METHODS_STATE);
    return token::WFD_RESPONSE_METHODS;
  }

^"Require: org.wfa.wfd1.0" {
    return token::WFD_SUPPORT_CHECK;
  }

^"Content-Type:" {
    BEGIN(CONTENT_TYPE_STATE);
    return token::WFD_CONTENT_TYPE;
  }

^"Content-Length:" {
    BEGIN(INITIAL);
    return token::WFD_CONTENT_LENGTH;
  }

^"Session:" {
    BEGIN(SESSION_STATE);
    return token::WFD_SESSION;
  }

^"Transport: RTP/AVP/UDP;unicast;client_port=" {
    return token::WFD_TRANSPORT;
  }

^[[:alpha:]][[:alnum:]\-\_]*":" {
    BEGIN(MATCH_STRING_STATE);
    yylval->sval = new std::string(yytext, yyleng - 1);
    return token::WFD_HEADER;
  }

<SESSION_STATE>[^ ;\t\r\n]+ {
    BEGIN(INITIAL);
    yylval->sval = new std::string(yytext, yyleng);
    return token::WFD_SESSION_ID;
  }

<MATCH_RESPONSE_CODE_STATE>{DIGITS} {
    BEGIN(MATCH_STRING_STATE);
    yylval->nval = atoi(yytext);
    return token::WFD_RESPONSE_CODE;
  }

<MATCH_STRING_STATE>[^ \r\n][^\r\n]+/"\r\n" {
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

<CONTENT_TYPE_STATE>[-[:alnum:]]+\/[-[:alnum:]]+ {
    BEGIN(INITIAL);
    yylval->sval = new std::string(yytext);
    return token::WFD_MIME;
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

  /* RTSP request rule,  e.g., OPTIONS * RTSP/1.0 */
"RTSP/"{DIGIT}"."{DIGIT} {
    return token::WFD_END;
  }

  /* GET_PARAMETER rtsp://localhost/wfd1.0 RTSP/1.0 */
"rtsp://"[^ \t\n]+ {
    yylval->sval = new std::string(yytext);
    return token::WFD_REQUEST_URI;
  }

 /* all unmatched */
<*>. {}
%%
