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

#include "gst_sink_media_manager.h"

GstSinkMediaManager::GstSinkMediaManager(const std::string& hostname)
  : gst_pipeline_(new MiracGstSink(hostname, 0)),
    optimal_format_ () {
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

std::pair<int,int> GstSinkMediaManager::ListeningRtpPorts() const {
  return std::pair<int,int>(gst_pipeline_->sink_udp_port(), 0);
}

void GstSinkMediaManager::SetPresentationUrl(const std::string& url) {
  presentation_url_ = url;
}

std::string GstSinkMediaManager::PresentationUrl() const {
  return presentation_url_;
}

void GstSinkMediaManager::SetSession(const std::string& session) {
  session_ = session;
}

std::string GstSinkMediaManager::Session() const {
  return session_;
}

std::vector<wfd::SupportedH264VideoFormats>
GstSinkMediaManager::GetSupportedH264VideoFormats() const {
  // declare that we support all resolutions, CHP and level 4.2
  // gstreamer should handle all of it :)
  std::vector<wfd::CEARatesAndResolutions> cea_rr;
  std::vector<wfd::VESARatesAndResolutions> vesa_rr;
  std::vector<wfd::HHRatesAndResolutions> hh_rr;
  wfd::RateAndResolution i;

  for (i = wfd::CEA640x480p60; i <= wfd::CEA1920x1080p24; i++)
      cea_rr.push_back(static_cast<wfd::CEARatesAndResolutions>(i));
  for (i = wfd::VESA800x600p30; i <= wfd::VESA1920x1200p30; i++)
      vesa_rr.push_back(static_cast<wfd::VESARatesAndResolutions>(i));
  for (i = wfd::HH800x480p30; i <= wfd::HH848x480p60; i++)
      hh_rr.push_back(static_cast<wfd::HHRatesAndResolutions>(i));
  return {wfd::SupportedH264VideoFormats(wfd::CHP, wfd::k4_2, cea_rr, vesa_rr, hh_rr),
          wfd::SupportedH264VideoFormats(wfd::CBP, wfd::k4_2, cea_rr, vesa_rr, hh_rr)};
}

wfd::NativeVideoFormat GstSinkMediaManager::SupportedNativeVideoFormat() const {
  // pick the maximum possible resolution, let gstreamer deal with it
  // TODO: get the actual screen size of the system
  return wfd::NativeVideoFormat(wfd::CEA1920x1080p60);
}

bool GstSinkMediaManager::SetOptimalFormat(
    const wfd::SelectableH264VideoFormat& optimal_format) {
  optimal_format_ = optimal_format;
  return true;
}


wfd::SelectableH264VideoFormat GstSinkMediaManager::GetOptimalFormat() const {
  return optimal_format_;
}
