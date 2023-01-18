// Copyright (C) 2021 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef __ANDROID__
#include <android-base/logging.h>
#include <log/log.h>
#endif

#include <ditto/logger.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <vector>

namespace dittosuite {

Logger& Logger::GetInstance() {
  static Logger logger;
  return logger;
}

void Logger::SetLogLevel(LOG_LEVEL log_level) {
  log_level_ = log_level;
}

void Logger::SetLogStream(LOG_STREAM log_stream) {
  log_stream_ = log_stream;
}

LOG_LEVEL Logger::GetLogLevel() const {
  return log_level_;
}

LOG_STREAM Logger::GetLogStream() const {
  return log_stream_;
}

std::string LogLevelToString(LOG_LEVEL log_level) {
  static const std::string prefixes[] = {"VERBOSE", "DEBUG", "INFO", "WARNING", "ERROR"};
  return prefixes[log_level];
}

#ifdef __ANDROID__
android::base::LogSeverity LogLevelToAndroidLogLevel(LOG_LEVEL log_level) {
  switch (log_level) {
    case LOG_LEVEL_VERBOSE:
      return android::base::VERBOSE;
      break;
    case LOG_LEVEL_DEBUG:
      return android::base::DEBUG;
      break;
    case LOG_LEVEL_INFO:
      return android::base::INFO;
      break;
    case LOG_LEVEL_WARNING:
      return android::base::WARNING;
      break;
    case LOG_LEVEL_ERROR:
      return android::base::ERROR;
      break;
    default:
      break;
  }
}
#endif

void Logger::WriteLogMessage(const std::string message, LOG_LEVEL log_level) {
  std::string message_to_print = LogLevelToString(log_level) + ": " + message;
  switch (log_stream_) {
    case LOG_STREAM_STDOUT:
      std::cout << message_to_print << '\n';
      break;
#ifdef __ANDROID__
    case LOG_STREAM_LOGCAT:
      LOG(LogLevelToAndroidLogLevel(log_level)) << message_to_print;
      break;
#endif
    default:
      break;
  }
}

}  // namespace dittosuite
