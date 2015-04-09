/*
 * This file is part of Wireless Display Software for Linux OS
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

#ifndef LOGGING_H_
#define LOGGING_H_

#include <cstdarg>

#include "wfd_export.h"

namespace wfd {

/**
 * WFD logging subsystem.
 */
class WFD_EXPORT LogSystem {
 public:
  typedef void (*LogFunction)(const char*, ...);

  /**
   * Sets a function to log a normal message.
   * @param function to log a normal message
   */
  static void set_log_func(LogFunction func) { log_func_ = func; }
  /**
   * Gets a function to log a normal message @see set_log_func
   * @return function to log a normal message
   */
  static LogFunction log_func() { return log_func_; }

  /**
   * Sets a function to log a verbose message.
   * @param function to log a verbose message
   */
  static void set_vlog_func(LogFunction func) { vlog_func_ = func; }
  /**
   * Gets a function to log a verbose message @see set_vlog_func
   * @return function to log a verbose message
   */
  static LogFunction vlog_func() { return vlog_func_; }

  /**
   * Sets a function to log a warning message.
   * @param function to log a warning message
   */
  static void set_warning_func(LogFunction func) { warning_func_ = func; }
  /**
   * Gets a function to log a warning message @see set_warning_func
   * @return function to log a warning message
   */
  static LogFunction warning_func() { return warning_func_; }

  /**
   * Sets a function to log an error message.
   * @param function to log an error message
   */
  static void set_error_func(LogFunction func) { error_func_ = func; }
  /**
   * Gets a function to log an error message @see set_error_func
   * @return function to log an error message
   */
  static LogFunction error_func() { return error_func_; }

 private:
  static LogFunction log_func_;
  static LogFunction vlog_func_;
  static LogFunction warning_func_;
  static LogFunction error_func_;

 private:
  LogSystem() = delete;
};

}

#define WFD_LOG(...) (*wfd::LogSystem::log_func())(__VA_ARGS__);
#define WFD_VLOG(...) (*wfd::LogSystem::vlog_func())(__VA_ARGS__);
#define WFD_WARNING(...) (*wfd::LogSystem::warning_func())(__VA_ARGS__);
#define WFD_ERROR(...) (*wfd::LogSystem::error_func())(__VA_ARGS__);

#endif // LOGGING_H_
