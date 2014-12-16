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

void DesktopMediaManager::SetRtpPorts(int port1, int port2) {
  gst_pipeline_.reset(new MiracGstTestSource(WFD_DESKTOP, hostname_, port1));
  gst_pipeline_->SetState(GST_STATE_READY);
}

std::pair<int, int> DesktopMediaManager::RtpPorts() const {
  return std::make_pair(gst_pipeline_->UdpSourcePort(), 0);
}

void DesktopMediaManager::SetPresentationUrls(const std::string& url0,
    const std::string& url1) {
}

std::pair<std::string, std::string> DesktopMediaManager::PresentationUrls() const {
  return std::pair<std::string, std::string>();
}

void DesktopMediaManager::SetSession(std::string& session) {
}

std::string DesktopMediaManager::Session() const {
  return std::string();
}
