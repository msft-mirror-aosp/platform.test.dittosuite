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

#include <ditto/instruction_factory.h>

#include <ditto/create_file.h>
#include <ditto/delete_file.h>
#include <ditto/instruction_set.h>
#include <ditto/logger.h>

namespace dittosuite {

std::unique_ptr<InstructionSet> InstructionFactory::CreateFromProtoInstructionSet(
    const int& repeat, const dittosuiteproto::InstructionSet& proto_instruction_set) {
  std::vector<std::unique_ptr<Instruction>> instructions;
  for (const auto& instruction : proto_instruction_set.instructions()) {
    instructions.push_back(std::move(InstructionFactory::CreateFromProtoInstruction(instruction)));
  }
  return std::make_unique<InstructionSet>(repeat, std::move(instructions));
}

std::unique_ptr<Instruction> InstructionFactory::CreateFromProtoInstruction(
    const dittosuiteproto::Instruction& proto_instruction) {
  switch (proto_instruction.instruction_oneof_case()) {
    case dittosuiteproto::Instruction::InstructionOneofCase::kInstructionSet:
      return InstructionFactory::CreateFromProtoInstructionSet(proto_instruction.repeat(),
                                                               proto_instruction.instruction_set());
    case dittosuiteproto::Instruction::InstructionOneofCase::kInstructionCreateFile:
      return std::make_unique<CreateFile>(proto_instruction.repeat(),
                                          proto_instruction.instruction_create_file().file());
    case dittosuiteproto::Instruction::InstructionOneofCase::kInstructionDeleteFile:
      return std::make_unique<DeleteFile>(proto_instruction.repeat(),
                                          proto_instruction.instruction_delete_file().file());
    case dittosuiteproto::Instruction::InstructionOneofCase::INSTRUCTION_ONEOF_NOT_SET:
      LOGE("Instruction was not set in .ditto file");
      return nullptr;
    default:
      LOGE("Invalid instruction was set in .ditto file");
      return nullptr;
  }
}

} // namespace dittosuite

