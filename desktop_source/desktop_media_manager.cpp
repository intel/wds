#include "desktop_media_manager.h"

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

std::pair<int, int> DesktopMediaManager::SinkRtpPorts() const {
  return std::pair<int, int>(sink_port1_, sink_port2_);
}

int DesktopMediaManager::SourceRtpPort() const {
  return gst_pipeline_->UdpSourcePort();
}

std::vector<wfd::SelectableH264VideoFormat>
DesktopMediaManager::GetSelectableH264VideoFormats() const {
  return {wfd::SelectableH264VideoFormat()};
}

bool DesktopMediaManager::SetOptimalFormat(
    const wfd::SelectableH264VideoFormat& optimal_format) {
  format_ = optimal_format;
  return true;
}

wfd::SelectableH264VideoFormat DesktopMediaManager::GetOptimalFormat() const {
  return format_;
}
