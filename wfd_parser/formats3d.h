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


#ifndef FORMATS3D_H_
#define FORMATS3D_H_

#include "property.h"

#include <vector>

namespace WFD {

// todo(shalamov): refactor, looks almost similar to VideoFormats

struct H264Codec3d {
 public:
  H264Codec3d(unsigned char profile, unsigned char level,
      unsigned long long int video_capability_3d, unsigned char latency,
      unsigned short min_slice_size, unsigned short slice_enc_params,
      unsigned char frame_rate_control_support, int max_hres,
      int max_vres)
    : profile_(profile),
      level_(level),
      video_capability_3d_(video_capability_3d),
      latency_(latency),
      min_slice_size_(min_slice_size),
      slice_enc_params_(slice_enc_params),
      frame_rate_control_support_(frame_rate_control_support),
      max_hres_(max_hres),
      max_vres_(max_vres) {}

  std::string to_string() const;

  unsigned char profile_;
  unsigned char level_;
  unsigned long long int video_capability_3d_;
  unsigned char latency_;
  unsigned short min_slice_size_;
  unsigned short slice_enc_params_;
  unsigned char frame_rate_control_support_;
  int max_hres_;
  int max_vres_;
};

typedef std::vector<WFD::H264Codec3d> H264Codecs3d;

class Formats3d: public Property {
 public:
  Formats3d();
  Formats3d(unsigned char native,
            unsigned char preferred_display_mode,
            const H264Codecs3d& h264_codecs_3d);
  virtual ~Formats3d();

  unsigned char native_resolution() const { return native_; }
  unsigned char preferred_display_mode() const { return preferred_display_mode_;}
  const H264Codecs3d& codecs() const { return h264_codecs_3d_; }

  virtual std::string to_string() const;

 private:
  unsigned char native_;
  unsigned char preferred_display_mode_;
  H264Codecs3d h264_codecs_3d_;
};

}  // namespace WFD

#endif  // FORMATS3D_H_
