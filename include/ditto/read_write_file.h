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

class ReadWriteFile : public Instruction {
 public:
  enum Type { kSequential, kRandom };
  enum Reseeding { kOnce, kEachRoundOfCycles, kEachCycle };

  explicit ReadWriteFile(const std::string& name, int repeat, int64_t size, int64_t block_size,
                         int64_t starting_offset, Type type, u_int32_t seed, Reseeding reseeding,
                         int input_fd_key);

 protected:
  virtual void SetUpSingle() override;
  virtual void RunSingle() override;

  int64_t size_;
  int64_t block_size_;
  int64_t starting_offset_;
  Type type_;
  std::mt19937_64 gen_;
  uint64_t seed_;
  Reseeding reseeding_;
  int input_fd_key_;

  struct Unit {
    int64_t count;
    int64_t offset;
  };

  std::vector<Unit> units_;
  std::unique_ptr<char[]> buffer_;

 private:
  void SetUp() override;
};

class WriteFile : public ReadWriteFile {
 public:
  inline static const std::string kName = "instruction_write_file";

  explicit WriteFile(int repeat, int64_t size, int64_t block_size, int64_t starting_offset,
                     Type type, u_int32_t seed, Reseeding reseeding, bool fsync, int input_fd_key);

 private:
  void RunSingle() override;

  bool fsync_;
};

class ReadFile : public ReadWriteFile {
 public:
  inline static const std::string kName = "instruction_read_file";

  enum ReadFAdvise { kAutomatic, kNormal, kSequential, kRandom };

  explicit ReadFile(int repeat, int64_t size, int64_t block_size, int64_t starting_offset,
                    Type type, u_int32_t seed, Reseeding reseeding, ReadFAdvise fadvise,
                    int input_fd_key);

 private:
  void SetUpSingle() override;
  void RunSingle() override;

  ReadFAdvise fadvise_;
};

}  // namespace dittosuite
