#ifndef DESKTOP_MEDIA_MANAGER_H_
#define DESKTOP_MEDIA_MANAGER_H_

#include <memory>

#include "wfd/public/media_manager.h"
#include "mirac-gst-test-source.hpp"

class DesktopMediaManager : public wfd::SourceMediaManager {
 public:
  explicit DesktopMediaManager(const std::string& hostname);
  void Play() override;
  void Pause() override;
  void Teardown() override;
  bool IsPaused() const override;
  void SetSinkRtpPorts(int port1, int port2) override;
  std::pair<int,int> SinkRtpPorts() const override;
  int GetLocalRtpPort() const override;

  std::vector<wfd::SelectableH264VideoFormat> GetSelectableH264VideoFormats() const override;
  bool SetOptimalVideoFormat(const wfd::SelectableH264VideoFormat& optimal_format) override;
  wfd::SelectableH264VideoFormat GetOptimalVideoFormat() const override;
  bool InitOptimalAudioFormat(const std::vector<wfd::AudioCodec>& sink_supported_codecs) override;
  wfd::AudioCodec GetOptimalAudioFormat() const override;
  void SendIDRPicture() override;

 private:
  std::string hostname_;
  std::unique_ptr<MiracGstTestSource> gst_pipeline_;
  int sink_port1_;
  int sink_port2_;
  wfd::SelectableH264VideoFormat format_;
};

#endif // DESKTOP_MEDIA_MANAGER_H_
