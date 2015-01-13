/*
 * This file is part of wysiwidi
 *
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef VIDEO_FORMAT_H_
#define VIDEO_FORMAT_H_

namespace wfd {

enum Resolution {
  // CEA resolutions
  k640x480,
  k720x480,
  k720x576,
  k1280x720,
  k1920x1080,

  // VESA Resolutions
  k800x600,
  k1024x768,
  k1152x864,
  k1280x768,
  k1280x800,
  k1360x768,
  k1366x768,
  k1280x1024,
  k1400x1050,
  k1440x900,
  k1600x900,
  k1600x1200,
  k1680x1024,
  k1680x1050,
  k1920x1200,

  // HH resolutions
  k800x480,
  k854x480,
  k864x480,
  k640x360,
  k960x540,
  k848x480
};

enum FrameRate {
  p24,
  p25,
  p30,
  p60,
  i50,
  i60
};


struct NativeVideoFormat {
  NativeVideoFormat(Resolution resolution, FrameRate frame_rate)
  : frame_rate(frame_rate), resolution(resolution) {}
  FrameRate frame_rate;
  Resolution resolution;
};

struct H264VideoFormat {
  enum H264Profile {
    CBP,
    CHP
  };

  enum H264Level {
    k3_1,
    k3_2,
    k4,
    k4_1,
    k4_2
  };

  // Resolution type
  enum {
    CEA,
    VESA,
    HH
  } type;

  H264VideoFormat(H264Profile profile, H264Level level,
      Resolution resolution, FrameRate frame_rate)
  : profile(profile),
    level(level),
    resolution(resolution),
    frame_rate(frame_rate) {
      if (resolution < k800x600)
        type = CEA;
      else if (resolution < k800x480)
        type = VESA;
      else
        type = HH;
  }

  H264Profile profile;
  H264Level level;
  Resolution resolution;
  FrameRate frame_rate;
};

}  // namespace wfd

#endif  // VIDEO_FORMAT_H_
