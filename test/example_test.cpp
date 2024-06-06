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

#include "include/instruction_test.h"

#include <dirent.h>

#include <ditto/parser.h>

bool string_has_suffix(const std::string& input, const std::string& suffix) {
  if (input.length() < suffix.length()) {
    return false;
  }
  return (!input.compare(input.length() - suffix.length(), suffix.length(), suffix));
}

/**
 * For compatibility with C++ versions prior to C++17 that do not support
 * `recursive_directory_iterator`, recurse the directory manually.
 */
void __ditto_files_paths(std::vector<std::string>* ditto_paths, const std::string& base_path) {
  struct dirent* entry;
  DIR* directory = opendir(base_path.c_str());

  std::cout << "Recursing in: " << base_path << std::endl;
  while ((entry = readdir(directory)) != nullptr) {
    std::string path = base_path + "/" + entry->d_name;
    std::cout << "Found: " << path << std::endl;
    switch (entry->d_type) {
      case DT_REG: {
        if (string_has_suffix(entry->d_name, ".ditto")) {
          ditto_paths->push_back(path);
        }
        break;
      }
      case DT_DIR: {
        if (std::string(".").compare(entry->d_name) && std::string("..").compare(entry->d_name)) {
          __ditto_files_paths(ditto_paths, path);
        }
        break;
      }
    }
  }

  closedir(directory);
}

std::vector<std::string> ditto_files_paths() {
  std::string base_path;
  std::vector<std::string> paths;

  if (absolute_path.empty()) {
    base_path = "example";
  } else {
    base_path = absolute_path + "/example";
  }

  __ditto_files_paths(&paths, base_path);
  return paths;
}

class ExampleTest : public testing::TestWithParam<std::string> {};

// Test each .ditto file inside example/ folder to make sure that all of them follow the schema
// If .ditto is parsed successfully, exit with code 0 is expected
TEST_P(ExampleTest, IsParsable) {
  std::string file_path = GetParam();
  std::cout << "Testing file path: " << GetParam() << std::endl;
#ifndef __ANDROID__
  if (std::string::npos != file_path.rfind("example/android/")) {
    GTEST_SKIP();
  }
#else
  EXPECT_EXIT(
      {
        dittosuite::Parser::GetParser().Parse(file_path, {});
        exit(0);
      },
      testing::ExitedWithCode(0), "");
#endif
}

INSTANTIATE_TEST_SUITE_P(DittoFile, ExampleTest, testing::ValuesIn(ditto_files_paths()));
