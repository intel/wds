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

#ifndef SINK_MEDIA_MANAGER_H_
#define SINK_MEDIA_MANAGER_H_

#include <memory>

#include "wfd/public/media_manager.h"
#include "mirac-gst-sink.hpp"

class SinkMediaManager : public wfd::MediaManager {
 public:
  explicit SinkMediaManager(const std::string& hostname);

  virtual void Play() override;
  virtual void Pause() override;
  virtual void Teardown() override;
  virtual bool IsPaused() const override;
  virtual void SetSinkRtpPorts(int port1, int port2) override;
  virtual std::pair<int,int> SinkRtpPorts() const override;
  virtual int SourceRtpPort() const override;
  virtual void SetPresentationUrl(const std::string& url) override;
  virtual std::string PresentationUrl() const override;
  virtual void SetSession(const std::string& session) override;
  virtual std::string Session() const override;

 private:
  std::string hostname_;
  std::string presentation_url_;
  std::string session_;
  std::unique_ptr<MiracGstSink> gst_pipeline_;
};

#endif // SINK_MEDIA_MANAGER_H_
