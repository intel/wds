#ifndef DESKTOP_SOURCE_H
#define DESKTOP_SOURCE_H

#include <memory>

#include "wfd/public/media_manager.h"
#include "wfd/public/source.h"

#include "mirac-broker.hpp"
#include "mirac-gst-test-source.hpp"

class DesktopSource : public MiracBroker {
 public:
  explicit DesktopSource(int rtsp_port);
  ~DesktopSource();

  wfd::Source* wfd_source() { return wfd_source_.get(); }

 private:
  virtual void got_message(const std::string& message) override;
  virtual void on_connected() override;

  std::unique_ptr<wfd::MediaManager> media_manager_;
  std::unique_ptr<wfd::Source> wfd_source_;
};

#endif // DESKTOP_SOURCE_H
