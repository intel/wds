#include "desktop_media_manager.h"
#include "mirac_broker_source.h"

#include "wfd/public/source.h"

MiracBrokerSource::MiracBrokerSource(int rtsp_port)
  : MiracBroker(std::to_string(rtsp_port)) {
}

MiracBrokerSource::~MiracBrokerSource() {}

void MiracBrokerSource::got_message(const std::string& message) {
  wfd_source_->RTSPDataReceived(message);
}

void MiracBrokerSource::on_connected() {
  media_manager_.reset(new DesktopMediaManager(get_peer_address()));
  wfd_source_.reset(wfd::Source::Create(this, media_manager_.get()));
  wfd_source_->Start();
}
