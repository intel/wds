#ifndef DESKTOP_MEDIA_MANAGER_H_
#define DESKTOP_MEDIA_MANAGER_H_

#include <memory>

#include "wfd/public/media_manager.h"
#include "mirac-gst-test-source.hpp"

class DesktopMediaManager : public wfd::SourceMediaManager {
 public:
  explicit DesktopMediaManager(const std::string& hostname);
  virtual void Play() override;
  virtual void Pause() override;
  virtual void Teardown() override;
  virtual bool IsPaused() const override;
  virtual void SetSinkRtpPorts(int port1, int port2) override;
  virtual std::pair<int,int> SinkRtpPorts() const override;
  virtual int SourceRtpPort() const override;

  virtual std::vector<wfd::H264VideoFormat> SupportedH264VideoFormats() const override;
  virtual wfd::NativeVideoFormat SupportedNativeVideoFormat() const override;

 private:
  std::string hostname_;
  std::unique_ptr<MiracGstTestSource> gst_pipeline_;

  int sink_port1_;
  int sink_port2_;
};

#endif // DESKTOP_MEDIA_MANAGER_H_
