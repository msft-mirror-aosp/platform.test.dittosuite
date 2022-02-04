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

#pragma once

#include <stdio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace dittosuite {

enum LOG_LEVEL {
  LOG_LEVEL_VERBOSE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL
};

enum LOG_STREAM { LOG_STREAM_STDOUT, LOG_STREAM_LOGCAT };

class Logger {
 public:
  Logger(Logger const&) = delete;
  void operator=(Logger const&) = delete;
  static Logger& GetInstance();
  void SetLogLevel(LOG_LEVEL log_level);
  void SetLogStream(LOG_STREAM log_stream);
  LOG_LEVEL GetLogLevel() const;
  LOG_STREAM GetLogStream() const;
  void WriteLogMessage(LOG_LEVEL log_level, const std::string& message,
                       const std::string& file_name, int line, bool print_errno);

 protected:
  Logger() {}

 private:
  LOG_LEVEL log_level_;
  LOG_STREAM log_stream_;
};

}  // namespace dittosuite

#define DITTO_LOGGER dittosuite::Logger::GetInstance()

#define DITTO_LOG(VERBOSITY, X, print_errno)                                                 \
  do {                                                                                       \
    if (DITTO_LOGGER.GetLogLevel() <= dittosuite::LOG_LEVEL_##VERBOSITY) {                   \
      DITTO_LOGGER.WriteLogMessage(dittosuite::LOG_LEVEL_##VERBOSITY, X, __FILE__, __LINE__, \
                                   print_errno);                                             \
    }                                                                                        \
  } while (false)

#define LOGF(X)                 \
  do {                          \
    DITTO_LOG(FATAL, X, false); \
    exit(EXIT_FAILURE);         \
  } while (false)
#define LOGE(X) DITTO_LOG(ERROR, X, false)
#define LOGW(X) DITTO_LOG(WARNING, X, false)
#define LOGI(X) DITTO_LOG(INFO, X, false)
#define LOGD(X) DITTO_LOG(DEBUG, X, false)
#define LOGV(X) DITTO_LOG(VERBOSE, X, false)

#define PLOGF(X)               \
  do {                         \
    DITTO_LOG(FATAL, X, true); \
    exit(EXIT_FAILURE);        \
  } while (false)
#define PLOGE(X) DITTO_LOG(ERROR, X, true)
#define PLOGW(X) DITTO_LOG(WARNING, X, true)
#define PLOGI(X) DITTO_LOG(INFO, X, true)
#define PLOGD(X) DITTO_LOG(DEBUG, X, true)
#define PLOGV(X) DITTO_LOG(VERBOSE, X, true)
