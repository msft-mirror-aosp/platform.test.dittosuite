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

#include <ditto/create_file.h>

#include <fcntl.h>
#include <unistd.h>

#include <fstream>

#include <ditto/shared_variables.h>

namespace dittosuite {

CreateFile::CreateFile(int repeat, const std::string& file)
    : Instruction(repeat), file_(Instruction::GetAbsolutePath() + file), output_fd_key_(-1) {}

void CreateFile::SetUp() {}

void CreateFile::RunSingle() {
  int fd = open(file_.c_str(), O_CREAT | O_CLOEXEC, O_RDWR);

  if (output_fd_key_ != -1) {
    SharedVariables::Set(output_fd_key_, fd);
  }
}

void CreateFile::TearDown() {}

int CreateFile::GetOutputFdKey() {
  return output_fd_key_;
}

void CreateFile::SetOutputFdKey(int output_fd_key) {
  output_fd_key_ = output_fd_key;
}

} // namespace dittosuite

