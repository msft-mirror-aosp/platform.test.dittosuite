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

#include <ditto/read_write_file.h>

#include <fcntl.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>

#include <ditto/logger.h>
#include <ditto/shared_variables.h>
#include <ditto/utils.h>

namespace dittosuite {

ReadWriteFile::ReadWriteFile(int repeat, int64_t size, int64_t block_size, ReadWriteType type,
                             u_int32_t seed, int input_fd_key)
    : Instruction(repeat),
      size_(size),
      block_size_(block_size),
      type_(type),
      gen_(seed),
      input_fd_key_(input_fd_key) {
  buffer_ = std::make_unique<char[]>(block_size_);
  std::fill(buffer_.get(), buffer_.get() + block_size_, 170);  // 170 = 10101010
}

void ReadWriteFile::SetUp() {
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

  Instruction::SetUp();
}

void ReadWriteFile::RunSingle() {}

WriteFile::WriteFile(int repeat, int64_t size, int64_t block_size, ReadWriteType type,
                     u_int32_t seed, bool fsync, int input_fd_key)
    : ReadWriteFile(repeat, size, block_size, type, seed, input_fd_key), fsync_(fsync) {}

void WriteFile::RunSingle() {
  int fd = std::get<int>(SharedVariables::Get(input_fd_key_));

  for (const auto& unit : units_) {
    if (pwrite64(fd, buffer_.get(), unit.count, unit.offset) == -1) {
      LOGE("Error while calling write()");
      exit(EXIT_FAILURE);
    }
  }

  if (fsync_ && fsync(fd) != 0) {
    LOGE("Error while calling fsync()");
    exit(EXIT_FAILURE);
  }
}

ReadFile::ReadFile(int repeat, int64_t size, int64_t block_size, ReadWriteType type, u_int32_t seed,
                   ReadFAdvise fadvise, int input_fd_key)
    : ReadWriteFile(repeat, size, block_size, type, seed, input_fd_key), fadvise_(fadvise) {}

void ReadFile::SetUp() {
  int fd = std::get<int>(SharedVariables::Get(input_fd_key_));
  int64_t file_size = GetFileSize(fd);

  int advise;
  switch (fadvise_) {
    case ReadFAdvise::kAutomatic: {
      switch (type_) {
        case ReadWriteType::kSequential: {
          advise = POSIX_FADV_SEQUENTIAL;
          break;
        }
        case ReadWriteType::kRandom: {
          advise = POSIX_FADV_RANDOM;
          break;
        }
      }
      break;
    }
    case ReadFAdvise::kNormal: {
      advise = POSIX_FADV_NORMAL;
      break;
    }
    case ReadFAdvise::kSequential: {
      advise = POSIX_FADV_SEQUENTIAL;
      break;
    }
    case ReadFAdvise::kRandom: {
      advise = POSIX_FADV_RANDOM;
      break;
    }
  }

  if (posix_fadvise64(fd, 0, file_size, advise) != 0) {
    LOGE("Error while calling fadvise()");
    exit(EXIT_FAILURE);
  }
  ReadWriteFile::SetUp();
}

void ReadFile::RunSingle() {
  int fd = std::get<int>(SharedVariables::Get(input_fd_key_));

  for (const auto& unit : units_) {
    if (pread64(fd, buffer_.get(), unit.count, unit.offset) == -1) {
      LOGE("Error while calling read()");
      exit(EXIT_FAILURE);
    }
  }
}

}  // namespace dittosuite
