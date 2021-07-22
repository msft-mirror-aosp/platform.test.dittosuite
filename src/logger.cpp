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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include <ditto/logger.h>

namespace dittosuite {

Logger& Logger::GetInstance() {
  static Logger logger;
  return logger;
}

void Logger::SetLogLevel(LOG_LEVEL log_level) {
  log_level_ = log_level;
}

LOG_LEVEL Logger::GetLogLevel() const {
  return log_level_;
}

std::string LogLevelToString(LOG_LEVEL log_level) {
  std::string prefixes[] = {"VERBOSE", "DEBUG", "INFO", "WARNING", "ERROR"};
  return prefixes[log_level];
}

void Logger::WriteLogMessage(const std::string message, LOG_LEVEL log_level) {
  std::cout << LogLevelToString(log_level) << ": " << message << '\n';
}

}  // namespace dittosuite
