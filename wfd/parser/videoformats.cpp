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

#include "videoformats.h"

#include <cassert>

#include "macros.h"

namespace wfd {

H264Codec::H264Codec(unsigned char profile, unsigned char level,
    unsigned int cea_support, unsigned int vesa_support,
    unsigned int hh_support, unsigned char latency,
    unsigned short min_slice_size, unsigned short slice_enc_params,
    unsigned char frame_rate_control_support,
    unsigned short max_hres, unsigned short max_vres)
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

H264Codec::H264Codec(H264VideoFormat format)
  : profile_(1 << format.profile),
    level_(1 << format.level),
    cea_support_((format.type == CEA) ? 1 << format.rate_resolution : 0),
    vesa_support_((format.type == VESA) ? 1 << format.rate_resolution : 0),
    hh_support_((format.type == HH) ? 1 << format.rate_resolution : 0),
    latency_(0),
    min_slice_size_(0),
    slice_enc_params_(0),
    frame_rate_control_support_(0),
    max_hres_(0),
    max_vres_(0) {

}

std::string H264Codec::ToString() const {
  std::string ret;
  MAKE_HEX_STRING_2(profile, profile_);
  MAKE_HEX_STRING_2(level, level_);
  MAKE_HEX_STRING_8(cea_support, cea_support_);
  MAKE_HEX_STRING_8(vesa_support, vesa_support_);
  MAKE_HEX_STRING_8(hh_support, hh_support_);
  MAKE_HEX_STRING_2(latency,latency_);
  MAKE_HEX_STRING_4(min_slice_size, min_slice_size_);
  MAKE_HEX_STRING_4(slice_enc_params, slice_enc_params_);
  MAKE_HEX_STRING_2(frame_rate_control_support, frame_rate_control_support_);

  ret = profile + std::string(SPACE)
      + level + std::string(SPACE)
      + cea_support + std::string(SPACE)
      + vesa_support + std::string(SPACE)
      + hh_support + std::string(SPACE)
      + latency + std::string(SPACE)
      + min_slice_size + std::string(SPACE)
      + slice_enc_params + std::string(SPACE)
      + frame_rate_control_support + std::string(SPACE);

  if (max_hres_ > 0) {
    MAKE_HEX_STRING_4(max_hres, max_hres_);
    ret += max_hres;
  } else {
    ret += NONE;
  }
  ret += std::string(SPACE);

  if (max_vres_ > 0) {
    MAKE_HEX_STRING_4(max_vres, max_vres_);
    ret += max_vres;
  } else {
    ret += NONE;
  }

  return ret;
}

namespace {

template <typename EnumType, typename ArgType>
EnumType MaskToEnum(ArgType from, EnumType biggest_value) {
  assert(from != 0);
  ArgType copy = from;
  unsigned result = 0;
  while ((copy & 1) == 0 && copy != 0) {
    copy = copy >> 1;
    ++result;
  }
  if (result > static_cast<unsigned>(biggest_value)) {
    assert(false);
    return biggest_value;
  }
  return static_cast<EnumType>(result);
}

template <typename EnumType, typename ArgType>
std::vector<EnumType> MaskToEnumList(ArgType from, EnumType biggest_value) {
  assert(from != 0);
  ArgType copy = from;
  unsigned enum_value = 0;
  std::vector<EnumType> result;

  while (copy != 0) {
    if ((copy & 1) != 0) {
      if (enum_value > static_cast<unsigned>(biggest_value))
        break;

      result.push_back(static_cast<EnumType>(enum_value));
    }
    copy = copy >> 1;
    ++enum_value;
  }

  return result;
}

inline H264VideoFormat::H264Profile ToH264Profile(unsigned char profile) {
  return MaskToEnum<H264VideoFormat::H264Profile>(profile, H264VideoFormat::CHP);
}

inline H264VideoFormat::H264Level ToH264Level(unsigned char level) {
  return MaskToEnum<H264VideoFormat::H264Level>(level, H264VideoFormat::k4_2);
}

}  // namespace

void H264Codec::ToVideoFormats(std::vector<H264VideoFormat>& formats) const {
  auto profile = ToH264Profile(profile_);
  auto level = ToH264Level(level_);
  if (cea_support_ != 0) {
    auto list = MaskToEnumList<CEARatesAndResolutions>(
        cea_support_, CEA1920x1080p24);
    for(auto rate_resolution: list)
      formats.push_back(H264VideoFormat(profile, level, rate_resolution));
  }
  if (vesa_support_ != 0) {
    auto list = MaskToEnumList<VESARatesAndResolutions>(
        vesa_support_, VESA1920x1200p30);
    for(auto rate_resolution: list)
      formats.push_back(H264VideoFormat(profile, level, rate_resolution));
  }
  if (hh_support_ != 0) {
    auto list = MaskToEnumList<HHRatesAndResolutions>(
        hh_support_, HH848x480p60);
    for(auto rate_resolution: list)
      formats.push_back(H264VideoFormat(profile, level, rate_resolution));
  }
}

VideoFormats::VideoFormats() : Property(WFD_VIDEO_FORMATS, true) {
}

VideoFormats::VideoFormats(NativeVideoFormat format,
    bool preferred_display_mode,
    const std::vector<H264VideoFormat>& h264_formats)
  : Property(WFD_VIDEO_FORMATS),
    preferred_display_mode_(preferred_display_mode ? 1 : 0) {
  native_ = (format.rate_resolution << 3) | format.type;
  for(auto h264_format : h264_formats)
    h264_codecs_.push_back(H264Codec(h264_format));
}

VideoFormats::VideoFormats(unsigned char native,
    unsigned char preferred_display_mode,
    const H264Codecs& h264_codecs)
  : Property(WFD_VIDEO_FORMATS),
    native_(native),
    preferred_display_mode_(preferred_display_mode),
    h264_codecs_(h264_codecs) {
}

VideoFormats::~VideoFormats() {
}

namespace {

template <typename EnumType>
NativeVideoFormat GetFormatFromIndex(unsigned index, EnumType biggest_value) {
  if (index <= static_cast<unsigned>(biggest_value))
    return NativeVideoFormat(static_cast<EnumType>(index));
  assert(false);
  return NativeVideoFormat(biggest_value);
}

}

NativeVideoFormat VideoFormats::GetNativeFormat() const {
  unsigned index  = native_ >> 3;
  unsigned selection_bits = native_ & 7;
  switch (selection_bits) {
  case 0: // 0b000 CEA
    return GetFormatFromIndex<CEARatesAndResolutions>(index, CEA1920x1080p24);
  case 1: // 0b001 VESA
    return GetFormatFromIndex<VESARatesAndResolutions>(index, VESA1920x1200p30);
  case 2: // 0b010 HH
    return GetFormatFromIndex<HHRatesAndResolutions>(index, HH848x480p60);
  default:
    assert(false);
    break;
  }
  return NativeVideoFormat(CEA640x480p60);
}

std::vector<H264VideoFormat> VideoFormats::GetSupportedH264Formats() const {
  std::vector<H264VideoFormat> result;
  for (const auto& codec : h264_codecs_)
    codec.ToVideoFormats(result);
  return result;
}

std::string VideoFormats::ToString() const {
  std::string ret;

  ret = PropertyName::wfd_video_formats
      + std::string(SEMICOLON)+ std::string(SPACE);

  if (is_none())
    return ret + wfd::NONE;

  MAKE_HEX_STRING_2(native, native_);
  MAKE_HEX_STRING_2(preferred_display_mode, preferred_display_mode_);

  ret += native + std::string(SPACE)
      + preferred_display_mode + std::string(SPACE);

  auto it = h264_codecs_.begin();
  auto end = h264_codecs_.end();
  while(it != end) {
    ret += (*it).ToString();
    ++it;
    if (it != end)
      ret += ", ";
  }

  return ret;
}

}  // namespace wfd
