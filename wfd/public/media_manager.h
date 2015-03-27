/*
 * This file is part of wysiwidi
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef MEDIA_MANAGER_H_
#define MEDIA_MANAGER_H_

#include <string>
#include <vector>
#include "audio_codec.h"
#include "video_format.h"
#include "wfd_export.h"

namespace wfd {

/**
 * MediaManager interface.
 *
 * Source or sink applications should implement that interface. MediaManager
 * instance is used by state machine to control media stream.
 */
class MediaManager {
 public:
  virtual ~MediaManager() {}

  /**
   * Triggers playback of the media stream.
   * Should be called only by the state machine.
   */
  virtual void Play() = 0;

  /**
   * Pauses playback of the media stream.
   * Should be called only by the state machine.
   */
  virtual void Pause() = 0;

  /**
   * Destroys media stream.
   * Should be called only by the state machine.
   */
  virtual void Teardown() = 0;

  /**
   * Queries whether media stream is paused.
   * @return true if media stream is paused, false otherwise.
   */
  virtual bool IsPaused() const = 0;

  /**
   * Sets optimal H264 format that would be used to send / receive video stream
   *
   * @param optimal H264 format
   * @return true if format can be used by media manager
   */
  virtual bool SetOptimalVideoFormat(const SelectableH264VideoFormat& optimal_format) = 0;
};

class SinkMediaManager : public MediaManager {
 public:
  /**
   * Returns RTP ports that are used to transmit media streams.
   * @see SetRtpPorts
   * @return pair of RTP ports, port0 and port1
   */
  virtual std::pair<int,int> ListeningRtpPorts() const = 0;

  /**
   * Sets presentation URL for media stream.
   * Presentation URL can be referred in order to control media stream resource
   * within wfd session.
   *
   * @param presentation url that represents video / audio stream
   */
  virtual void SetPresentationUrl(const std::string& url) = 0;

  /**
   * Returns presentation URL for managed media resource.
   * @see SetPresentationUrl
   * @return presentation url
   */
  virtual std::string PresentationUrl() const = 0;

  /**
   * Sets unique ID for wfd session.
   * @param string that uniquely identifies wfd session
   */
  virtual void SetSession(const std::string& session) = 0;

  /**
   * Returns unique wfd session id.
   * @return unique id for wfd session
   */
  virtual std::string Session() const = 0;

  /**
   * Returns list of supported H264 video formats
   * @return vector of supported H264 video formats
   */
  virtual std::vector<SupportedH264VideoFormats> GetSupportedH264VideoFormats() const = 0;

  /**
   * Returns native video format of a device
   * @return native video format
   */
  virtual NativeVideoFormat SupportedNativeVideoFormat() const = 0;
};

class WFD_EXPORT SourceMediaManager : public MediaManager {
 public:
  /**
   * Sets RTP ports for media stream.
   * In case of coupled sink configuration video and audio data could be sent
   * to different RTP ports.
   *
   * When MediaManager is responsible for receiving media stream, SetRtpPorts
   * must set listening RTP ports for UDP connection. MediaManager that is
   * sending media stream, must use RTP ports for outgoing UDP connection.
   *
   * @param port0 RTP port for video / audio stream
   * @param port1 RTP port that could be used to send audio stream
   */
  virtual void SetSinkRtpPorts(int port1, int port2) = 0;

  /**
   * Returns RTP ports that are used to transmit media streams.
   * @see SetRtpPorts
   * @return pair of RTP ports, port0 and port1
   */
  virtual std::pair<int,int> SinkRtpPorts() const = 0;

  /**
   * Returns the source RTP port
   * @return RTP port
   */
  virtual int GetLocalRtpPort() const = 0;

  /**
   * Returns list of supported H264 video formats
   * @return vector of supported H264 video formats
   */
  virtual std::vector<SelectableH264VideoFormat> GetSelectableH264VideoFormats() const = 0;

  /**
   * Finds optimal format for streaming.
   * Default quality selection algorithm will pick codec with higher bandwidth
   *
   * @param native format of a remote device
   * @param list of H264 formats that are supported by remote device
   * @return optimal H264 video format
   */
  virtual SelectableH264VideoFormat FindOptimalVideoFormat(
      const NativeVideoFormat& remote_device_native_format,
      const std::vector<SelectableH264VideoFormat>& remotely_supported_formats) const;

  /**
   * Gets optimal H264 format @see SetOptimalVideoFormat
   *
   * @return optimal H264 format
   */
  virtual SelectableH264VideoFormat GetOptimalVideoFormat() const = 0;

  /**
   * Initializes optimal audio codec
   * The optimal audio codec will be returned by GetOptimalAudioFormat
   *
   * @param sink_supported_codecs list of the codecs supported by sink
   * @return true if optimal codec can be found among the given list items, false otherwise
   */
  virtual bool InitOptimalAudioFormat(const std::vector<AudioCodec>& sink_supported_codecs) = 0;

  /**
   * Gets optimal audio codec @see InitOptimalAudioFormat
   *
   * @return optimal audio codec
   */
  virtual AudioCodec GetOptimalAudioFormat() const = 0;

  /**
   * Sends of H.264 instantaneous decoding refresh (IDR) picture
   * to recover the content streaming.
   */
  virtual void SendIDRPicture() = 0;
};

inline SourceMediaManager* ToSourceMediaManager(MediaManager* mng) {
  return static_cast<SourceMediaManager*>(mng);
}

inline SinkMediaManager* ToSinkMediaManager(MediaManager* mng) {
  return static_cast<SinkMediaManager*>(mng);
}

}  // namespace wfd

#endif // MEDIA_MANAGER_H_

