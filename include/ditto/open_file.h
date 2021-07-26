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

#include <string>

#include <ditto/instruction.h>

namespace dittosuite {

class OpenFile : public Instruction {
 public:
  explicit OpenFile(int repeat, const std::string& file);

  void SetUp() override;
  void TearDown() override;

  int GetOutputFdKey();
  void SetOutputFdKey(int output_fd_key);

 private:
  void RunSingle() override;

  std::string file_;
  int output_fd_key_;
};

}  // namespace dittosuite