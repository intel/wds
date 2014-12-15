#ifndef DESKTOP_MEDIA_MANAGER_H_
#define DESKTOP_MEDIA_MANAGER_H_

#include <memory>

#include "wfd/public/media_manager.h"
#include "mirac-gst-test-source.hpp"

class DesktopMediaManager : public wfd::MediaManager {
 public:
  explicit DesktopMediaManager(const std::string& hostname);
  virtual void Play() override;
  virtual void Pause() override;
  virtual void Teardown() override;
  virtual bool IsPaused() const override;
  virtual void SetRtpPorts(int port0, int port1) override;
  virtual std::pair<int, int> RtpPorts() const override;
  virtual void SetPresentationUrls(const std::string& url0,
      const std::string& url1) override;
  virtual std::pair<std::string, std::string> PresentationUrls() const override;
  virtual void SetSession(std::string& session) override;
  virtual std::string Session() const override;

 private:
  std::string hostname_;
  std::unique_ptr<MiracGstTestSource> gst_pipeline_;
};

#endif // DESKTOP_MEDIA_MANAGER_H_
