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

#include <vector>

namespace wfd {

typedef unsigned RateAndResolution;

// NOTE : Do not change the elements order in the following enums!

enum ResolutionType {
  CEA,
  VESA,
  HH
};

enum CEARatesAndResolutions {
  CEA640x480p60,
  CEA720x480p60,
  CEA720x480i60,
  CEA720x576p50,
  CEA720x576i50,
  CEA1280x720p30,
  CEA1280x720p60,
  CEA1920x1080p30,
  CEA1920x1080p60,
  CEA1920x1080i60,
  CEA1280x720p25,
  CEA1280x720p50,
  CEA1920x1080p25,
  CEA1920x1080p50,
  CEA1920x1080i50,
  CEA1280x720p24,
  CEA1920x1080p24
};

enum VESARatesAndResolutions {
  VESA800x600p30,
  VESA800x600p60,
  VESA1024x768p30,
  VESA1024x768p60,
  VESA1152x864p30,
  VESA1152x864p60,
  VESA1280x768p30,
  VESA1280x768p60,
  VESA1280x800p30,
  VESA1280x800p60,
  VESA1360x768p30,
  VESA1360x768p60,
  VESA1366x768p30,
  VESA1366x768p60,
  VESA1280x1024p30,
  VESA1280x1024p60,
  VESA1400x1050p30,
  VESA1400x1050p60,
  VESA1440x900p30,
  VESA1440x900p60,
  VESA1600x900p30,
  VESA1600x900p60,
  VESA1600x1200p30,
  VESA1600x1200p60,
  VESA1680x1024p30,
  VESA1680x1024p60,
  VESA1680x1050p30,
  VESA1680x1050p60,
  VESA1920x1200p30
};

enum HHRatesAndResolutions {
  HH800x480p30,
  HH800x480p60,
  HH854x480p30,
  HH854x480p60,
  HH864x480p30,
  HH864x480p60,
  HH640x360p30,
  HH640x360p60,
  HH960x540p30,
  HH960x540p60,
  HH848x480p30,
  HH848x480p60
};

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


struct NativeVideoFormat {
  NativeVideoFormat()
  : type(CEA), rate_resolution(CEA640x480p60) {}
  NativeVideoFormat(CEARatesAndResolutions rr)
  : type(CEA), rate_resolution(rr) {}
  NativeVideoFormat(VESARatesAndResolutions rr)
  : type(VESA), rate_resolution(rr) {}
  NativeVideoFormat(HHRatesAndResolutions rr)
  : type(HH), rate_resolution(rr) {}

  ResolutionType type;
  RateAndResolution rate_resolution;
};

struct SelectableH264VideoFormat {
  SelectableH264VideoFormat()
  : profile(CBP), level(k3_1), type(CEA), rate_resolution(CEA640x480p60) {}

  SelectableH264VideoFormat(H264Profile profile, H264Level level, CEARatesAndResolutions rr)
  : profile(profile), level(level), type(CEA), rate_resolution(rr) {}

  SelectableH264VideoFormat(H264Profile profile, H264Level level, VESARatesAndResolutions rr)
  : profile(profile), level(level), type(VESA), rate_resolution(rr) {}

  SelectableH264VideoFormat(H264Profile profile, H264Level level, HHRatesAndResolutions rr)
  : profile(profile), level(level), type(HH), rate_resolution(rr) {}

  H264Profile profile;
  H264Level level;
  ResolutionType type;
  RateAndResolution rate_resolution;
};

struct SupportedH264VideoFormats {
  SupportedH264VideoFormats()
  : profile(CBP), level(k3_1), cea_rr({CEA640x480p60}) {}

  SupportedH264VideoFormats(H264Profile profile, H264Level level,
                  std::vector<CEARatesAndResolutions> cea,
                  std::vector<VESARatesAndResolutions> vesa,
                  std::vector<HHRatesAndResolutions> hh
                 )
  : profile(profile), level(level), cea_rr(cea), vesa_rr(vesa), hh_rr(hh) {}

  H264Profile profile;
  H264Level level;
  std::vector<CEARatesAndResolutions> cea_rr;
  std::vector<VESARatesAndResolutions> vesa_rr;
  std::vector<HHRatesAndResolutions> hh_rr;
};


}  // namespace wfd

#endif  // VIDEO_FORMAT_H_
