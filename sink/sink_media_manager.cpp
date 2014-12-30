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
  gst_pipeline_->Play();
}

void SinkMediaManager::Pause() {
  gst_pipeline_->Pause();
}

void SinkMediaManager::Teardown() {
  gst_pipeline_->Teardown();
}

bool SinkMediaManager::IsPaused() const {
  return gst_pipeline_->IsPaused();
}

void SinkMediaManager::SetSinkRtpPorts(int port1, int port2) {
}

std::pair<int,int> SinkMediaManager::SinkRtpPorts() const {
  return std::pair<int,int>(gst_pipeline_->sink_udp_port(), 0);
}

int SinkMediaManager::SourceRtpPort() const {
  return 0;
}


void SinkMediaManager::SetPresentationUrl(const std::string& url) {
  presentation_url_ = url;
}

std::string SinkMediaManager::PresentationUrl() const {
  return presentation_url_;
}

void SinkMediaManager::SetSession(const std::string& session) {
  session_ = session;
}

std::string SinkMediaManager::Session() const {
  return session_;
}
