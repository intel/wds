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

#include "gst_sink_media_manager.h"

GstSinkMediaManager::GstSinkMediaManager(const std::string& hostname)
  : gst_pipeline_(new MiracGstSink(hostname, 0)) {
}

void GstSinkMediaManager::Play() {
  gst_pipeline_->Play();
}

void GstSinkMediaManager::Pause() {
  gst_pipeline_->Pause();
}

void GstSinkMediaManager::Teardown() {
  gst_pipeline_->Teardown();
}

bool GstSinkMediaManager::IsPaused() const {
  return gst_pipeline_->IsPaused();
}

std::pair<int,int> GstSinkMediaManager::GetLocalRtpPorts() const {
  return std::pair<int,int>(gst_pipeline_->sink_udp_port(), 0);
}

void GstSinkMediaManager::SetPresentationUrl(const std::string& url) {
  presentation_url_ = url;
}

std::string GstSinkMediaManager::GetPresentationUrl() const {
  return presentation_url_;
}

void GstSinkMediaManager::SetSessionId(const std::string& session) {
  session_ = session;
}

std::string GstSinkMediaManager::GetSessionId() const {
  return session_;
}

std::vector<wds::SupportedH264VideoFormats>
GstSinkMediaManager::GetSupportedH264VideoFormats() const {
  // declare that we support all resolutions, CHP and level 4.2
  // gstreamer should handle all of it :)
  std::vector<wds::CEARatesAndResolutions> cea_rr;
  std::vector<wds::VESARatesAndResolutions> vesa_rr;
  std::vector<wds::HHRatesAndResolutions> hh_rr;
  wds::RateAndResolution i;

  for (i = wds::CEA640x480p60; i <= wds::CEA1920x1080p24; i++)
      cea_rr.push_back(static_cast<wds::CEARatesAndResolutions>(i));
  for (i = wds::VESA800x600p30; i <= wds::VESA1920x1200p30; i++)
      vesa_rr.push_back(static_cast<wds::VESARatesAndResolutions>(i));
  for (i = wds::HH800x480p30; i <= wds::HH848x480p60; i++)
      hh_rr.push_back(static_cast<wds::HHRatesAndResolutions>(i));
  return {wds::SupportedH264VideoFormats(wds::CHP, wds::k4_2, cea_rr, vesa_rr, hh_rr),
          wds::SupportedH264VideoFormats(wds::CBP, wds::k4_2, cea_rr, vesa_rr, hh_rr)};
}

wds::NativeVideoFormat GstSinkMediaManager::GetSupportedNativeVideoFormat() const {
  // pick the maximum possible resolution, let gstreamer deal with it
  // TODO: get the actual screen size of the system
  return wds::NativeVideoFormat(wds::CEA1920x1080p60);
}

bool GstSinkMediaManager::SetOptimalVideoFormat(
    const wds::SelectableH264VideoFormat& optimal_format) {
  return true;
}
