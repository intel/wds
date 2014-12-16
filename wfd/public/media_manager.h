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
#include <utility>

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
  virtual void SetRtpPorts(int port0, int port1 = 0) = 0;

  /**
   * Returns RTP ports that are used to transmit media streams.
   * @see SetRtpPorts
   * @return pair of RTP ports, port0 and port1
   */
  virtual std::pair<int, int> RtpPorts() const = 0;

  /**
   * Sets presentation URLs for media stream.
   * Presentation URL can be referred in order to control media stream resource
   * within wfd session. In coupled sink operation mode, url1 represents audio
   * resource.
   *
   * @param presentation url0 that represents video / audio stream
   * @param presentation url1 that represents audio stream resource
   */
  virtual void SetPresentationUrls(const std::string& url0,
      const std::string& url1 = std::string()) = 0;

  /**
   * Returns presentation URLs for managed media resource.
   * @see SetPresentationUrls
   * @return pair of URL strings, url0 and url1
   */
  virtual std::pair<std::string, std::string> PresentationUrls() const = 0;

  /**
   * Sets unique ID for wfd session.
   * @param string that uniquely identifies wfd session
   */
  virtual void SetSession(std::string& session) = 0;

  /**
   * Returns unique wfd session id.
   * @return unique id for wfd session
   */
  virtual std::string Session() const = 0;
};

}  // namespace wfd

#endif // MEDIA_MANAGER_H_

