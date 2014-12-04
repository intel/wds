#include "desktop_media_manager.h"
#include "desktop_source.h"

DesktopSource::DesktopSource(int rtsp_port)
  : MiracBroker(std::to_string(rtsp_port)) {
}

DesktopSource::~DesktopSource() {}


void DesktopSource::got_message(const std::string& message) {
  wfd_source_->RTSPDataReceived(message);
}

void DesktopSource::on_connected() {
  media_manager_.reset(new DesktopMediaManager(get_peer_address()));
  wfd_source_.reset(wfd::Source::Create(this, media_manager_.get()));
  wfd_source_->Start();
}
