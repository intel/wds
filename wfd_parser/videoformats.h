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


#ifndef VIDEOFORMATS_H_
#define VIDEOFORMATS_H_

#include "property.h"

#include <vector>

namespace WFD {

struct H264Codec {
 public:
  H264Codec(unsigned char profile, unsigned char level,
      unsigned int cea_support, unsigned int vesa_support,
      unsigned int hh_support, unsigned char latency,
      unsigned short min_slice_size, unsigned short slice_enc_params,
      unsigned char frame_rate_control_support, int max_hres,
      int max_vres)
    : profile_(profile),
      level_(level),
      cea_support_(cea_support),
      vesa_support_(vesa_support),
      hh_support_(hh_support),
      latency_(latency),
      min_slice_size_(min_slice_size),
      slice_enc_params_(slice_enc_params),
      frame_rate_control_support_(frame_rate_control_support),
      max_hres_(max_hres),
      max_vres_(max_vres) {}

  std::string to_string() const;

  unsigned char profile_;
  unsigned char level_;
  unsigned int cea_support_;
  unsigned int vesa_support_;
  unsigned int hh_support_;
  unsigned char latency_;
  unsigned short min_slice_size_;
  unsigned short slice_enc_params_;
  unsigned char frame_rate_control_support_;
  int max_hres_;
  int max_vres_;
};

typedef std::vector<WFD::H264Codec> H264Codecs;

class VideoFormats: public Property {

public:
  VideoFormats();
  VideoFormats(unsigned char native,
               unsigned char preferred_display_mode,
               const H264Codecs& h264_codecs);
  virtual ~VideoFormats();

  unsigned char native_resolution() const { return native_; }
  unsigned char preferred_display_mode() const { return preferred_display_mode_;}
  const H264Codecs& h264_codecs() const { return h264_codecs_; }

  virtual std::string to_string() const;

 private:
  unsigned char native_;
  unsigned char preferred_display_mode_;
  H264Codecs h264_codecs_;
};

}  // namespace WFD

#endif  // VIDEOFORMATS_H_
