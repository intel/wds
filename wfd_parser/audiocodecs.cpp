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


#include "audiocodecs.h"

#include <assert.h>

#include "macros.h"

namespace WFD {

AudioCodec::AudioCodec(const AudioFormat::Type& audio_format,
    const AudioFormat::Modes& audio_modes, unsigned short latency) {
  set_audio_format(audio_format);
  set_audio_modes(audio_modes);
  set_latency(latency);
}

AudioCodec::~AudioCodec(){
}

const AudioFormat::Type AudioCodec::audio_format() const {
  assert(audio_format_ >= AudioFormat::Type::LPCM
      && audio_format_ <= AudioFormat::Type::AC3);
  return audio_format_;
}
void AudioCodec::set_audio_format(AudioFormat::Type audio_format) {
  assert(audio_format >= AudioFormat::Type::LPCM
      && audio_format <= AudioFormat::Type::AC3);
  audio_format_ = audio_format;
}

AudioFormat::Modes AudioCodec::audio_modes() const {
  return audio_modes_;
}

void AudioCodec::set_audio_modes(AudioFormat::Modes audio_modes) {
  assert(audio_modes.size() == AudioFormat::ModesCount);
  audio_modes_ = audio_modes;
}

unsigned short AudioCodec::latency() const {
  return latency_;
}
void AudioCodec::set_latency(unsigned short latency) {
  latency_ = latency;
}

std::string AudioCodec::to_string() const {
  MAKE_HEX_STRING_8(audio_modes_str,
      static_cast<unsigned int>(audio_modes().to_ulong()));
  MAKE_HEX_STRING_2(latency_str, latency());
  std::string ret = AudioFormat::name[audio_format()]
    + std::string(SPACE) + audio_modes_str
    + std::string(SPACE) + latency_str;
  return ret;
}

AudioCodecs::AudioCodecs() : Property(WFD_AUDIO_CODECS, true) {
}

AudioCodecs::AudioCodecs(const std::vector<AudioCodec>& audio_codecs)
  : Property(WFD_AUDIO_CODECS),
    audio_codecs_(audio_codecs) {
}

AudioCodecs::~AudioCodecs() {
}

std::string AudioCodecs::to_string() const {
  std::string ret = PropertyName::wfd_audio_codecs + std::string(SEMICOLON)
    + std::string(SPACE);

  if (audio_codecs_.size()) {
    auto i = audio_codecs_.begin();
    auto end = audio_codecs_.end();
    while (i != end) {
      ret += (*i).to_string();
      if (i != --audio_codecs_.end()) {
        ret += ", ";
      }
      ++i;
    }
  } else {
    ret += WFD::NONE;
  }

  return ret;
}

}  // namespace WFD
