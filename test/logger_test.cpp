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

#include <ditto/logger.h>
#include <gtest/gtest.h>

#include <vector>

namespace dittosuite {

TEST(DittoLogger, SimpleLog) {
  for (int i = 10; i <= 11; i++) {
    Logger::GetInstance().SetLogLevel(LOG_LEVEL_ERROR);
    std::cout << "My log level:  " << Logger::GetInstance().GetLogLevel() << '\n';
    LOGW("This is a warning");
    LOGE("This is an error");
    LOGI("This is an info");
    Logger::GetInstance().SetLogStream(LOG_STREAM_LOGCAT);
    Logger::GetInstance().SetLogLevel(LOG_LEVEL_WARNING);
    LOGW("This is a warning");
    LOGE("This is an error");
    LOGI("This is an info");
  }
}

TEST(DittoLogger, SetAndGetLevel) {
  for (const auto l :
       {LOG_LEVEL_VERBOSE, LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR}) {
    Logger::GetInstance().SetLogLevel(l);
    ASSERT_EQ(Logger::GetInstance().GetLogLevel(), l);
  }
}

TEST(DittoLogger, SetAndGetStream) {
  for (const auto s : {LOG_STREAM_STDOUT, LOG_STREAM_LOGCAT}) {
    Logger::GetInstance().SetLogStream(s);
    ASSERT_EQ(Logger::GetInstance().GetLogStream(), s);
  }
}

TEST(DittoLogger, LoggerPError) {
  FILE* pFile;
  std::string file_name = "nonexistent.txt";
  pFile = fopen(file_name.c_str(), "re");
  if (pFile == nullptr) {
    PLOGE("Cannot open \"" + file_name + "\"");
  }
}

}  // namespace dittosuite
