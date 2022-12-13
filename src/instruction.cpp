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

#include <ditto/instruction.h>

namespace dittosuite {

Instruction::Instruction(const std::string& name, int repeat) : name_(name), repeat_(repeat) {}

void Instruction::SetUp() {
  time_sampler_.MeasureStart();
}

void Instruction::Run() {
  for (int i = 0; i < repeat_; i++) {
    RunSingle();
  }
}

void Instruction::TearDown() {
  time_sampler_.MeasureEnd();
}

void Instruction::SetAbsolutePathKey(int absolute_path_key) {
  absolute_path_key_ = absolute_path_key;
}

int Instruction::GetAbsolutePathKey() {
  return absolute_path_key_;
}

int Instruction::absolute_path_key_;

}  // namespace dittosuite
