#ifndef DESKTOP_MEDIA_MANAGER_H_
#define DESKTOP_MEDIA_MANAGER_H_

#include <memory>

#include "libwds/public/media_manager.h"
#include "mirac-gst-test-source.hpp"

class DesktopMediaManager : public wds::SourceMediaManager {
 public:
  explicit DesktopMediaManager(const std::string& hostname);
  void Play() override;
  void Pause() override;
  void Teardown() override;
  bool IsPaused() const override;
  void SetSinkRtpPorts(int port1, int port2) override;
  std::pair<int,int> GetSinkRtpPorts() const override;
  int GetLocalRtpPort() const override;

  std::vector<wds::SelectableH264VideoFormat> GetSelectableH264VideoFormats() const override;
  bool InitOptimalVideoFormat(
      const wds::NativeVideoFormat& sink_native_format,
      const std::vector<wds::SelectableH264VideoFormat>& sink_supported_formats) override;
  wds::SelectableH264VideoFormat GetOptimalVideoFormat() const override;
  bool InitOptimalAudioFormat(const std::vector<wds::AudioCodec>& sink_supported_codecs) override;
  wds::AudioCodec GetOptimalAudioFormat() const override;
  void SendIDRPicture() override;

 private:
  std::string hostname_;
  std::unique_ptr<MiracGstTestSource> gst_pipeline_;
  int sink_port1_;
  int sink_port2_;
  wds::SelectableH264VideoFormat format_;
};

#endif // DESKTOP_MEDIA_MANAGER_H_
