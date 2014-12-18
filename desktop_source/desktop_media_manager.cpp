#include "desktop_media_manager.h"

#include <cassert>

DesktopMediaManager::DesktopMediaManager(const std::string& hostname)
  : hostname_(hostname) {
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
  assert(gst_pipeline_);
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

void DesktopMediaManager::SetPresentationUrl(const std::string& url) {
}

std::string DesktopMediaManager::PresentationUrl() const {
  return std::string();
}

void DesktopMediaManager::SetSession(const std::string& session) {
}

std::string DesktopMediaManager::Session() const {
  return std::string();
}
