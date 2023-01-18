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

#include <ditto/logger.h>
#include <ditto/shared_variables.h>
#include <ditto/utils.h>

namespace dittosuite {

OpenFile::OpenFile(SyscallInterface& syscall, int repeat, const std::string& path_name, bool create,
                   int output_fd_key)
    : Instruction(syscall, kName, repeat),
      random_name_(false),
      path_name_(GetAbsolutePath() + path_name),
      create_(create),
      input_key_(-1),
      output_fd_key_(output_fd_key) {}

OpenFile::OpenFile(SyscallInterface& syscall, int repeat, int input_key, bool create,
                   int output_fd_key)
    : Instruction(syscall, kName, repeat),
      random_name_(false),
      create_(create),
      input_key_(input_key),
      output_fd_key_(output_fd_key) {}

OpenFile::OpenFile(SyscallInterface& syscall, int repeat, bool create, int output_fd_key)
    : Instruction(syscall, kName, repeat),
      random_name_(true),
      create_(create),
      input_key_(-1),
      output_fd_key_(output_fd_key),
      gen_(time(nullptr)) {}

void OpenFile::SetUpSingle() {
  if (input_key_ != -1) {
    path_name_ = std::get<std::string>(SharedVariables::Get(input_key_));
  } else if (random_name_) {
    std::uniform_int_distribution<> uniform_distribution(1e8, 9e8);  // 9 digit number
    do {
      path_name_ = GetAbsolutePath() + std::to_string(uniform_distribution(gen_));
    } while (FileExists(syscall_, path_name_));
  }
  Instruction::SetUpSingle();
}

void OpenFile::RunSingle() {
  int fd = syscall_.Open(path_name_, create_);

  if (fd == -1) {
    LOGF("Error while trying to open the file");
  }

  if (output_fd_key_ != -1) {
    SharedVariables::Set(output_fd_key_, fd);
  }
}

}  // namespace dittosuite
