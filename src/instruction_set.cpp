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

#include <variant>

#include <ditto/logger.h>
#include <ditto/shared_variables.h>

namespace dittosuite {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

InstructionSet::InstructionSet(SyscallInterface& syscall, int repeat,
                               std::vector<std::unique_ptr<Instruction>> instructions, int list_key,
                               int item_key, AccessType type, Reseeding reseeding, uint32_t seed)
    : Instruction(syscall, kName, repeat),
      instructions_(std::move(instructions)),
      list_key_(list_key),
      item_key_(item_key),
      type_(type),
      reseeding_(reseeding),
      seed_(seed),
      gen_(seed) {}

InstructionSet::InstructionSet(SyscallInterface& syscall, int repeat,
                               std::vector<std::unique_ptr<Instruction>> instructions)
    : Instruction(syscall, kName, repeat),
      instructions_(std::move(instructions)),
      list_key_(-1),
      item_key_(-1) {}

void InstructionSet::SetUp() {
  if (reseeding_ == kEachRoundOfCycles) {
    gen_.seed(seed_);
  }
  Instruction::SetUp();
}

void InstructionSet::SetUpSingle() {
  if (reseeding_ == kEachCycle) {
    gen_.seed(seed_);
  }
  Instruction::SetUpSingle();
}

void InstructionSet::RunSingle() {
  if (list_key_ != -1 && item_key_ != -1) {
    std::visit(overloaded{[&](const std::vector<std::string>& list) {
                            std::uniform_int_distribution<> uniform_distribution(0,
                                                                                 list.size() - 1);
                            for (unsigned int i = 0; i < list.size(); ++i) {
                              switch (type_) {
                                case kSequential: {
                                  SharedVariables::Set(item_key_, list[i]);
                                  break;
                                }
                                case kRandom: {
                                  SharedVariables::Set(item_key_, list[uniform_distribution(gen_)]);
                                  break;
                                }
                              }
                              RunInstructions();
                            }
                          },
                          [](int) {
                            LOGE("Input for InstructionSet is not iterable.");
                            exit(EXIT_FAILURE);
                          },
                          [](const std::string&) {
                            LOGE("Input for InstructionSet is not iterable.");
                            exit(EXIT_FAILURE);
                          }},
               SharedVariables::Get(list_key_));
  } else {
    RunInstructions();
  }
}

void InstructionSet::RunInstructions() {
  for (const auto& instruction : instructions_) {
    instruction->SetUp();
    instruction->Run();
    instruction->TearDown();
  }
}

std::unique_ptr<Result> InstructionSet::CollectResults(const std::string& prefix) {
  auto result = std::make_unique<Result>(prefix + name_);
  result->AddMeasurement("duration", TimespecToNs(time_sampler_.GetSamples()));
  for (const auto& instruction : instructions_) {
    result->AddSubResult(instruction->CollectResults(""));
  }
  result->Analyse();
  return result;
}

}  // namespace dittosuite
