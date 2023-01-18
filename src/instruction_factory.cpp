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

namespace dittosuite {
  std::unique_ptr<Instruction> InstructionFactory::createFromProtoInstruction(
      const dittosuiteproto::Instruction& proto_instruction) {
    switch (proto_instruction.instruction_oneof_case()) {
        case dittosuiteproto::Instruction::InstructionOneofCase::kInstructionCreateFile:
          return std::make_unique<CreateFile>(proto_instruction.instruction_create_file().file());
        case dittosuiteproto::Instruction::InstructionOneofCase::INSTRUCTION_ONEOF_NOT_SET:
        default:
          // TODO: LOG() << "Invalid Instruction";
          return nullptr;
      }
  }

} // namespace dittosuite

