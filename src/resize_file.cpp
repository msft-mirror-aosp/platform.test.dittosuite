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

#include <ditto/resize_file.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>

#include <ditto/logger.h>
#include <ditto/shared_variables.h>

namespace dittosuite {

ResizeFile::ResizeFile(int repeat, int64_t size)
    : Instruction(repeat), size_(size), input_fd_key_(-1) {}

void ResizeFile::SetUp() {}

void ResizeFile::RunSingle() {
  int fd = std::get<int>(SharedVariables::Get(input_fd_key_));
  int64_t file_size = GetFileSize(fd);

  if (size_ > file_size && fallocate(fd, 0, 0, size_) != 0) {
    LOGE("Error while calling fallocate()");
    exit(EXIT_FAILURE);
  } else if (ftruncate(fd, size_) != 0) {
    LOGE("Error while calling ftruncate()");
    exit(EXIT_FAILURE);
  }
}

void ResizeFile::TearDown() {}

int ResizeFile::GetInputFdKey() {
  return input_fd_key_;
}

void ResizeFile::SetInputFdKey(int input_fd_key) {
  input_fd_key_ = input_fd_key;
}

int64_t ResizeFile::GetFileSize(int fd) {
  struct stat64 sb;
  fstat64(fd, &sb);
  return sb.st_size;
}

}  // namespace dittosuite