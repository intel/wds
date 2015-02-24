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

#ifndef RTSP_STATUS_CODE_H_
#define RTSP_STATUS_CODE_H_

namespace wfd {

enum RTSPStatusCode {
  RTSP_OK = 200,
  RTSP_SeeOther = 303,
  RTSP_NotAcceptable = 406,
  RTSP_UnsupportedMediaType = 415,
  RTSP_NotImplemented = 501
};

}  // namespace wfd

#endif
