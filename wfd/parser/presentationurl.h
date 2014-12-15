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


#ifndef PRESENTATIONURL_H_
#define PRESENTATIONURL_H_

#include "property.h"

namespace wfd {

class PresentationUrl: public Property {
 public:
  PresentationUrl(const std::string& presentation_url_1,
      const std::string presentation_url_2);
  virtual ~PresentationUrl();

  const std::string& presentation_url_1() const { return presentation_url_1_; }
  const std::string& presentation_url_2() const { return presentation_url_2_; }
  virtual std::string to_string() const override;

 private:
  std::string presentation_url_1_;
  std::string presentation_url_2_;
};

}  // namespace wfd

#endif  // PRESENTATIONURL_H_
