#include "desktop_media_manager.h"
#include "mirac-glib-logging.hpp"
#include <cassert>

DesktopMediaManager::DesktopMediaManager(const std::string& hostname)
  : hostname_(hostname),
    format_() {
}

void DesktopMediaManager::Play() {
  assert(gst_pipeline_);
  gst_pipeline_->SetState(GST_STATE_PLAYING);
}

void DesktopMediaManager::Pause() {
  assert(gst_pipeline_);
  gst_pipeline_->SetState(GST_STATE_PAUSED);
}

void DesktopMediaManager::Teardown() {
  if (gst_pipeline_)
    gst_pipeline_->SetState(GST_STATE_READY);
}

bool DesktopMediaManager::IsPaused() const {
  return (gst_pipeline_->GetState() != GST_STATE_PLAYING);
}

void DesktopMediaManager::SetSinkRtpPorts(int port1, int port2) {
  sink_port1_ = port1;
  sink_port2_ = port2;
  gst_pipeline_.reset(new MiracGstTestSource(WFD_DESKTOP, hostname_, port1));
  gst_pipeline_->SetState(GST_STATE_READY);
}

std::pair<int, int> DesktopMediaManager::GetSinkRtpPorts() const {
  return std::pair<int, int>(sink_port1_, sink_port2_);
}

int DesktopMediaManager::GetLocalRtpPort() const {
  return gst_pipeline_->UdpSourcePort();
}

std::vector<wds::SelectableH264VideoFormat>
DesktopMediaManager::GetSelectableH264VideoFormats() const {
  static std::vector<wds::SelectableH264VideoFormat> formats;
  if (formats.empty()) {
    wds::RateAndResolution i;
    for (i = wds::CEA640x480p60; i <= wds::CEA1920x1080p24; i++)
        formats.push_back(wds::SelectableH264VideoFormat(wds::CHP, wds::k4_2, static_cast<wds::CEARatesAndResolutions>(i)));
    for (i = wds::VESA800x600p30; i <= wds::VESA1920x1200p30; i++)
        formats.push_back(wds::SelectableH264VideoFormat(wds::CHP, wds::k4_2, static_cast<wds::VESARatesAndResolutions>(i)));
    for (i = wds::HH800x480p30; i <= wds::HH848x480p60; i++)
        formats.push_back(wds::SelectableH264VideoFormat(wds::CHP, wds::k4_2, static_cast<wds::HHRatesAndResolutions>(i)));
  }

  return formats;
}

bool DesktopMediaManager::InitOptimalVideoFormat(
    const wds::NativeVideoFormat& sink_native_format,
    const std::vector<wds::SelectableH264VideoFormat>& sink_supported_formats) {

  format_ = wds::FindOptimalVideoFormat(sink_native_format,
                                        GetSelectableH264VideoFormats(),
                                        sink_supported_formats);
  return true;
}

wds::SelectableH264VideoFormat DesktopMediaManager::GetOptimalVideoFormat() const {
  return format_;
}

bool DesktopMediaManager::InitOptimalAudioFormat(const std::vector<wds::AudioCodec>& sink_codecs) {
  for (const auto& codec : sink_codecs) {
     if (codec.format == wds::AAC && codec.modes.test(wds::AAC_48K_16B_2CH))
       return true;
  }
  return false;
}

wds::AudioCodec DesktopMediaManager::GetOptimalAudioFormat() const {
  wds::AudioModes audio_modes;
  audio_modes.set(wds::AAC_48K_16B_2CH);

  return wds::AudioCodec(wds::AAC, audio_modes, 0);
}

void DesktopMediaManager::SendIDRPicture() {
  WDS_WARNING("Unimplemented IDR picture request");
}
