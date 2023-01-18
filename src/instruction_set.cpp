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

#include <ditto/instruction_set.h>

namespace dittosuite {

InstructionSet::InstructionSet(int repeat, std::vector<std::unique_ptr<Instruction>> instructions)
    : Instruction(kName, repeat), instructions_(std::move(instructions)) {}

void InstructionSet::SetUp() {
  Instruction::SetUp();
}

void InstructionSet::RunSingle() {
  for (const auto& instruction : instructions_) {
    instruction->SetUp();
    instruction->Run();
    instruction->TearDown();
  }
}

void InstructionSet::TearDown() {
  Instruction::TearDown();
}

std::unique_ptr<Result> InstructionSet::CollectResults() {
  auto result = std::make_unique<Result>(name_, time_sampler_.GetTimeSamples());
  for (const auto& instruction : instructions_) {
    result->AddSubResult(instruction->CollectResults());
  }
  result->Analyse();
  return result;
}

}  // namespace dittosuite
