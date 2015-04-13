/*
 * This file is part of Wireless Display Software for Linux OS
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
#include "libwds/public/audio_codec.h"

#include <vector>

namespace wds {
namespace rtsp {

class AudioCodecs: public Property {
 public:
  AudioCodecs();
  AudioCodecs(const std::vector<wds::AudioCodec>& audio_codecs);
  ~AudioCodecs() override;

  const std::vector<wds::AudioCodec>& audio_codecs() const { return audio_codecs_; }
  std::string ToString() const override;

 private:
  std::vector<AudioCodec> audio_codecs_;
};

}  // namespace rtsp
}  // namespace wds

#endif  // AUDIOCODECS_H_
