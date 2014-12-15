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


#ifndef AUDIOCODECS_H_
#define AUDIOCODECS_H_

#include "property.h"

#include <vector>
#include <bitset>

namespace wfd {

namespace AudioFormat {
  enum Type {
    LPCM,
    AAC,
    AC3
  };

  const size_t ModesCount = 32;
  typedef std::bitset<ModesCount> Modes;

  enum Mode {
    MODE_44_1K_16B_2CH = 1 << 0,
    MODE_48K_16B_2CH = 1 << 1
  };

  const char* const name[] = {"LPCM", "AAC", "AC3"};
}

struct AudioCodec {
 public:
  AudioCodec(const AudioFormat::Type& audio_format,
      const AudioFormat::Modes& audio_modes, unsigned short latency);
  ~AudioCodec();

  const AudioFormat::Type audio_format() const;
  void set_audio_format(AudioFormat::Type audio_format);

  AudioFormat::Modes audio_modes() const;
  void set_audio_modes(AudioFormat::Modes audio_modes);

  unsigned short latency() const;
  void set_latency(unsigned short latency);

  std::string to_string() const;

 private:
  AudioFormat::Type audio_format_;
  AudioFormat::Modes audio_modes_;
  unsigned short latency_;
};

class AudioCodecs: public Property {
 public:
  AudioCodecs();
  AudioCodecs(const std::vector<AudioCodec>& audio_codecs);
  virtual ~AudioCodecs();

  const std::vector<AudioCodec>& audio_codecs() const { return audio_codecs_; }
  virtual std::string to_string() const override;

 private:
  std::vector<AudioCodec> audio_codecs_;
};

}  // namespace wfd

#endif  // AUDIOCODECS_H_
