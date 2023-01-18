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

#include <iostream>
#include <string>
#include <vector>

namespace dittosuite {
enum LOG_LEVEL {
  LOG_LEVEL_VERBOSE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR
};

class Logger {
 public:
  Logger(Logger const&) = delete;
  void operator=(Logger const&) = delete;
  static Logger& GetInstance();
  void SetLogLevel(LOG_LEVEL log_level);
  LOG_LEVEL GetLogLevel() const;
  void WriteLogMessage(const std::string message, const std::string prefix);
  LOG_LEVEL log_level_;

 protected:
  Logger() {}
};

#define LOGE(X)                                              \
  if (Logger::GetInstance().log_level_ <= LOG_LEVEL_ERROR) { \
    Logger::GetInstance().WriteLogMessage(X, "ERROR");       \
  }
#define LOGW(X)                                                \
  if (Logger::GetInstance().log_level_ <= LOG_LEVEL_WARNING) { \
    Logger::GetInstance().WriteLogMessage(X, "WARNING");       \
  }
#define LOGI(X)                                             \
  if (Logger::GetInstance().log_level_ <= LOG_LEVEL_INFO) { \
    Logger::GetInstance().WriteLogMessage(X, "INFO");       \
  }
#define LOGD(X)                                              \
  if (Logger::GetInstance().log_level_ <= LOG_LEVEL_DEBUG) { \
    Logger::GetInstance().WriteLogMessage(X, "DEBUG");       \
  }
#define LOGV(X)                                                \
  if (Logger::GetInstance().log_level_ <= LOG_LEVEL_VERBOSE) { \
    Logger::GetInstance().WriteLogMessage(X, "VERBOSE");       \
  }

}  // namespace dittosuite
