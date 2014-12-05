#include "desktop_media_manager.h"

#include <cassert>

DesktopMediaManager::DesktopMediaManager(const std::string& hostname)
  : hostname_(hostname) {
}

void DesktopMediaManager::Play() {
  gst_pipeline_->SetState(GST_STATE_PLAYING);
}

void DesktopMediaManager::Pause() {
}

void DesktopMediaManager::Teardown() {
}

bool DesktopMediaManager::IsPaused() const {
  return true;
}

void DesktopMediaManager::SetRtpPorts(int port1, int port2) {
  gst_pipeline_.reset(new MiracGstTestSource(WFD_DESKTOP, hostname_, port1));
  gst_pipeline_->SetState(GST_STATE_READY);
}

int DesktopMediaManager::RtpPort() const {
  return gst_pipeline_->UdpSourcePort();
}

void DesktopMediaManager::SetPresentationUrl(const std::string& url) {
}

std::string DesktopMediaManager::PresentationUrl() const {
  return std::string();
}

void DesktopMediaManager::SetSession(std::string& session) {
}

std::string DesktopMediaManager::Session() const {
  return std::string();
}
