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


#include "driver.h"

int  main(const int argc, const char **argv)
{
  WFD::Driver driver;
  std::string message_header("RTSP/1.0 200 OK\nCSeq: 2\r\n"
                             "Date: Sun, Aug 21 2011 04:20:53 GMT\r\n"
                             "Public: org.wfa.wfd1.0, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER\r\n"
                             "Require: org.wfa.wfd1.0");
  driver.parse_header(message_header);
  std::string message_payload("wfd_audio_codecs: LPCM 00000003 00, AC3 00000003 02");
  driver.parse_payload(message_payload);

  std::shared_ptr<WFD::Message> wfd_message(driver.parsed_message());
  std::cout << wfd_message->to_string() << std::endl;
}
