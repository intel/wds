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

#include "sink_media_manager.h"

SinkMediaManager::SinkMediaManager(const std::string& hostname)
  : gst_pipeline_(new MiracGstSink(hostname, 0)) {
}

void SinkMediaManager::Play() {
}

void SinkMediaManager::Pause() {
}

void SinkMediaManager::Teardown() {
}

bool SinkMediaManager::IsPaused() const {
  return true;
}

void SinkMediaManager::SetRtpPorts(int port0, int port1) {
}

std::pair<int, int> SinkMediaManager::RtpPorts() const {
  return std::make_pair(gst_pipeline_->sink_udp_port(), 0);
}

void SinkMediaManager::SetPresentationUrls(const std::string& url0,
    const std::string& url1) {
  presentation_url_ = url0;
}

std::pair<std::string, std::string> SinkMediaManager::PresentationUrls() const {
  return std::make_pair(presentation_url_, std::string());
}

void SinkMediaManager::SetSession(std::string& session) {
  session_ = session;
}

std::string SinkMediaManager::Session() const {
  return session_;
}
