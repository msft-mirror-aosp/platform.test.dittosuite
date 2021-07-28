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

#pragma once

#include <sys/types.h>

#include <cstdint>
#include <memory>
#include <random>
#include <vector>

#include <ditto/instruction.h>

namespace dittosuite {

class WriteFile : public Instruction {
 public:
  explicit WriteFile(int repeat, int64_t size, int64_t block_size, ReadWriteType type,
                     u_int32_t seed, bool fsync, int input_fd_key);

  void SetUp() override;
  void TearDown() override;

 private:
  void RunSingle() override;

  int64_t size_;
  int64_t block_size_;
  ReadWriteType type_;
  std::mt19937_64 gen_;
  bool fsync_;
  int input_fd_key_;

  struct Unit {
    int64_t count;
    int64_t offset;
  };

  std::vector<Unit> units_;
  std::unique_ptr<char[]> buffer_;
};

}  // namespace dittosuite
