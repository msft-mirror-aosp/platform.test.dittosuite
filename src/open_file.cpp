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

#include <ditto/open_file.h>

#include <fcntl.h>
#include <unistd.h>

#include <fstream>

#include <ditto/shared_variables.h>

namespace dittosuite {

OpenFile::OpenFile(int repeat, const std::string& file)
    : Instruction(repeat),
      file_(std::get<std::string>(SharedVariables::Get(Instruction::GetAbsolutePathKey())) + file),
      output_fd_key_(-1) {}

void OpenFile::SetUp() {}

void OpenFile::RunSingle() {
  int fd = open(file_.c_str(), O_CREAT | O_CLOEXEC, O_RDWR);

  if (output_fd_key_ != -1) {
    SharedVariables::Set(output_fd_key_, fd);
  }
}

void OpenFile::TearDown() {}

int OpenFile::GetOutputFdKey() {
  return output_fd_key_;
}

void OpenFile::SetOutputFdKey(int output_fd_key) {
  output_fd_key_ = output_fd_key;
}

}  // namespace dittosuite
