#ifndef DESKTOP_MEDIA_MANAGER_H_
#define DESKTOP_MEDIA_MANAGER_H_

#include <memory>

#include "media_manager.h"
#include "mirac-gst-test-source.hpp"

class DesktopMediaManager : public wfd::MediaManager {
 public:
  explicit DesktopMediaManager(const std::string& hostname);
  virtual void Play() override;
  virtual void Pause() override;
  virtual void Teardown() override;
  virtual bool IsPaused() const override;
  virtual void SetRtpPorts(int port1, int port2) override;
 private:
  std::string hostname_;
  std::unique_ptr<MiracGstTestSource> gst_pipeline_;
};

#endif // DESKTOP_MEDIA_MANAGER_H_
