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
  void WriteLogMessage(const std::string message, LOG_LEVEL log_level);
  void WriteLogErrorMessage(const std::string& message, LOG_LEVEL log_level);

 protected:
  Logger() {}

 private:
  LOG_LEVEL log_level_;
  LOG_STREAM log_stream_;
};

#define LOGF(X)                                                   \
  do {                                                            \
    if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_FATAL) { \
      Logger::GetInstance().WriteLogMessage(X, LOG_LEVEL_FATAL);  \
    }                                                             \
    exit(EXIT_FAILURE);                                           \
  } while (false)

#define LOGE(X)                                                 \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_ERROR) { \
    Logger::GetInstance().WriteLogMessage(X, LOG_LEVEL_ERROR);  \
  }
#define LOGW(X)                                                   \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_WARNING) { \
    Logger::GetInstance().WriteLogMessage(X, LOG_LEVEL_WARNING);  \
  }
#define LOGI(X)                                                \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_INFO) { \
    Logger::GetInstance().WriteLogMessage(X, LOG_LEVEL_INFO);  \
  }
#define LOGD(X)                                                 \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_DEBUG) { \
    Logger::GetInstance().WriteLogMessage(X, LOG_LEVEL_DEBUG);  \
  }
#define LOGV(X)                                                   \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_VERBOSE) { \
    Logger::GetInstance().WriteLogMessage(X, LOG_LEVEL_VERBOSE);  \
  }

#define PLOGF(X)                                                      \
  do {                                                                \
    if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_FATAL) {     \
      Logger::GetInstance().WriteLogErrorMessage(X, LOG_LEVEL_FATAL); \
    }                                                                 \
    exit(EXIT_FAILURE);                                               \
  } while (false)
#define PLOGE(X)                                                    \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_ERROR) {     \
    Logger::GetInstance().WriteLogErrorMessage(X, LOG_LEVEL_ERROR); \
  }
#define PLOGW(X)                                                      \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_WARNING) {     \
    Logger::GetInstance().WriteLogErrorMessage(X, LOG_LEVEL_WARNING); \
  }
#define PLOGI(X)                                                   \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_INFO) {     \
    Logger::GetInstance().WriteLogErrorMessage(X, LOG_LEVEL_INFO); \
  }
#define PLOGD(X)                                                    \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_DEBUG) {     \
    Logger::GetInstance().WriteLogErrorMessage(X, LOG_LEVEL_DEBUG); \
  }
#define PLOGV(X)                                                      \
  if (Logger::GetInstance().GetLogLevel() <= LOG_LEVEL_VERBOSE) {     \
    Logger::GetInstance().WriteLogErrorMessage(X, LOG_LEVEL_VERBOSE); \
  }

}  // namespace dittosuite