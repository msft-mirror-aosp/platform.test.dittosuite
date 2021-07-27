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

#include <ditto/write_file.h>

#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>

#include <ditto/logger.h>
#include <ditto/shared_variables.h>

namespace dittosuite {

WriteFile::WriteFile(int repeat, int64_t size, int64_t block_size, ReadWriteType type,
                     u_int32_t seed)
    : Instruction(repeat),
      size_(size),
      block_size_(block_size),
      type_(type),
      gen_(seed),
      input_fd_key_(-1) {
  buffer_ = std::make_unique<char[]>(block_size_);
  std::fill(buffer_.get(), buffer_.get() + block_size_, 170);  // 170 = 10101010
}

void WriteFile::SetUp() {
  int fd = std::get<int>(SharedVariables::Get(input_fd_key_));
  int64_t file_size = GetFileSize(fd);

  if (block_size_ > file_size) {
    LOGW("Supplied block_size is greater than total file size");
    return;
  }

  switch (type_) {
    case ReadWriteType::kSequential: {
      int64_t offset = 0;
      for (int64_t i = 0; i < (size_ / block_size_); i++) {
        if (offset > file_size - block_size_) {
          offset = 0;
        }
        units_.push_back({block_size_, offset});
        offset += block_size_;
      }
      break;
    }
    case ReadWriteType::kRandom: {
      std::uniform_int_distribution<> uniform_distribution(0, file_size - block_size_);

      for (int64_t i = 0; i < (size_ / block_size_); i++) {
        units_.push_back({block_size_, uniform_distribution(gen_)});
      }
      break;
    }
  }
}

void WriteFile::RunSingle() {
  int fd = std::get<int>(SharedVariables::Get(input_fd_key_));

  for (const auto& unit : units_) {
    pwrite(fd, buffer_.get(), unit.count, unit.offset);
  }
}

void WriteFile::TearDown() {}

int WriteFile::GetInputFdKey() {
  return input_fd_key_;
}

void WriteFile::SetInputFdKey(int input_fd_key) {
  input_fd_key_ = input_fd_key;
}

int64_t WriteFile::GetFileSize(int fd) {
  struct stat64 sb;
  fstat64(fd, &sb);
  return sb.st_size;
}

}  // namespace dittosuite
