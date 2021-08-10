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

#include <dirent.h>

#include <vector>

#include <ditto/logger.h>
#include <ditto/read_directory.h>
#include <ditto/shared_variables.h>

namespace dittosuite {

ReadDirectory::ReadDirectory(int repeat, const std::string& directory_name, int output_key)
    : Instruction(kName, repeat), directory_name_(directory_name), output_key_(output_key) {}

void ReadDirectory::RunSingle() {
  std::vector<std::string> output;

  DIR* directory = opendir(directory_name_.c_str());

  if (directory == nullptr) {
    LOGE("Error while calling opendir(). Directory name: " + directory_name_);
    exit(EXIT_FAILURE);
  }

  struct dirent* entry;
  while ((entry = readdir(directory)) != nullptr) {
    // Only collect regular files
    if (entry->d_type == DT_REG) {
      output.push_back(entry->d_name);
    }
  }
  SharedVariables::Set(output_key_, output);

  closedir(directory);
}

}  // namespace dittosuite