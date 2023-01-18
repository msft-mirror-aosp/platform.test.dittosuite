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
  void WriteLogMessage(const std::string& message, LOG_LEVEL log_level);
  void WriteLogErrorMessage(const std::string& message, LOG_LEVEL log_level);

 protected:
  Logger() {}

 private:
  LOG_LEVEL log_level_;
  LOG_STREAM log_stream_;
};

}  // namespace dittosuite

#define DITTO_LOGGER dittosuite::Logger::GetInstance()

#define DITTO_LOG(VERBOSITY, X)                                            \
  do {                                                                     \
    if (DITTO_LOGGER.GetLogLevel() <= dittosuite::LOG_LEVEL_##VERBOSITY) { \
      DITTO_LOGGER.WriteLogMessage(X, dittosuite::LOG_LEVEL_##VERBOSITY);  \
    }                                                                      \
  } while (false)

#define LOGF(X)          \
  do {                   \
    DITTO_LOG(FATAL, X); \
    exit(EXIT_FAILURE);  \
  } while (false)
#define LOGE(X) DITTO_LOG(ERROR, X)
#define LOGW(X) DITTO_LOG(WARNING, X)
#define LOGI(X) DITTO_LOG(INFO, X)
#define LOGD(X) DITTO_LOG(DEBUG, X)
#define LOGV(X) DITTO_LOG(VERBOSE, X)

#define DITTO_PLOG(VERBOSITY, X)                                               \
  do {                                                                         \
    if (DITTO_LOGGER.GetLogLevel() <= dittosuite::LOG_LEVEL_##VERBOSITY) {     \
      DITTO_LOGGER.WriteLogErrorMessage(X, dittosuite::LOG_LEVEL_##VERBOSITY); \
    }                                                                          \
  } while (false)

#define PLOGF(X)          \
  do {                    \
    DITTO_PLOG(FATAL, X); \
    exit(EXIT_FAILURE);   \
  } while (false)
#define PLOGE(X) DITTO_PLOG(ERROR, X)
#define PLOGW(X) DITTO_PLOG(WARNING, X)
#define PLOGI(X) DITTO_PLOG(INFO, X)
#define PLOGD(X) DITTO_PLOG(DEBUG, X)
#define PLOGV(X) DITTO_PLOG(VERBOSE, X)
