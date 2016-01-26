/*
 * This file is part of Wireless Display Software for Linux OS
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <string.h>
#include <iostream>
#include <fstream>

#include "libwds/rtsp/driver.h"
#include "libwds/rtsp/message.h"

using wds::rtsp::Driver;

namespace {
const char kTestHeaderCommand[] = "--header";
const char kTestPayloadCommand[] = "--payload";
const char kTestErrorCommand[] = "--error";
const char kTestNumLinesCommand[] = "--num-lines=";
const char kTestTestCaseCommand[] = "--test-case=";

const char kDefaultReplyHeader[] = "RTSP/1.0 200 OK\r\n"
                                   "CSeq: 2\r\n"
                                   "Content-Type: text/parameters\r\n"
                                   "Content-Length: 535\r\n\r\n";

const char kDefaultReplyErrorHeader[] = "RTSP/1.0 303 OK\r\n"
                                        "CSeq: 0\r\n"
                                        "Content-Type: text/parameters\r\n"
                                        "Content-Length: 55\r\n\r\n";

int PrintError(const char* program) {
  std::cerr << "Usage: " << program << " --test-case=ABSOLUTE_PATH_FILE [--header, --payload, --error]" << std::endl;
  std::cerr << "Usage: " << program << " --num-lines=NUM [--header, --payload, --error]" << std::endl;
  std::cerr << "Example: " << program << " --num-lines=6 --header" << std::endl;
  std::cerr << "Example: " << program << " --test-case=test-options-request.txt --header" << std::endl;
  return 1;
}

std::string GetBufferFromStdin(int num_lines) {
  std::string buffer, input;
  while(num_lines--) {
    getline(std::cin, input);
    buffer += input;
  }
  return buffer;
}

std::string GetBufferFromFile(const std::string& file) {
  std::string line, buffer;
  std::ifstream input_stream(file);
  if (input_stream.is_open()) {
    while (getline(input_stream, line))
      buffer += line;
    input_stream.close();
  }
  return buffer;
}

}  // namespace

int main(const int argc, const char **argv)
{
  // Program name, number of lines to be read, type of message
  if (argc < 3)
    return PrintError(argv[0]);

  std::string buffer;
  std::string filename;
  std::string arg(argv[1]);

  if (arg.compare(0, strlen(kTestNumLinesCommand), kTestNumLinesCommand) == 0)
    buffer = GetBufferFromStdin(atoi(arg.substr(strlen(kTestNumLinesCommand)).c_str()));
  else if (arg.compare(0, strlen(kTestTestCaseCommand), kTestTestCaseCommand) == 0)
    buffer = GetBufferFromFile(arg.substr(strlen(kTestNumLinesCommand)));
  else
    return PrintError(argv[0]);

  std::unique_ptr<wds::rtsp::Message> message;

  if (std::string(argv[2]) == kTestHeaderCommand) {
    Driver::Parse(buffer, message);
  } else if (std::string(argv[2]) == kTestPayloadCommand) {
    Driver::Parse(kDefaultReplyHeader, message);
    Driver::Parse(buffer, message);
  } else if (std::string(argv[2]) == kTestErrorCommand) {
    Driver::Parse(kDefaultReplyErrorHeader, message);
    Driver::Parse(buffer, message);
  } else {
    return PrintError(argv[0]);
  }

  return 0;
}
