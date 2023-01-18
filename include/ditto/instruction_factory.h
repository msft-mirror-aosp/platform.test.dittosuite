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
#include <ditto/instruction_set.h>
#include <ditto/read_write_file.h>

#ifdef __ANDROID__
#include "test/dittosuite/schema/benchmark.pb.h"
#else
#include "schema/benchmark.pb.h"
#endif

namespace dittosuite {

class InstructionFactory {
 public:
  static std::unique_ptr<InstructionSet> CreateFromProtoInstructionSet(
      const int& repeat, const dittosuiteproto::InstructionSet& proto_instruction_set);
  static std::unique_ptr<Instruction> CreateFromProtoInstruction(
      const dittosuiteproto::Instruction& proto_instruction);

 private:
  InstructionFactory();

  static Reseeding ConvertReseeding(const dittosuiteproto::Reseeding& proto_reseeding);
  static AccessType ConvertAccessType(const dittosuiteproto::AccessType& proto_type);
  static int ConvertReadFAdvise(
      const AccessType& type,
      const dittosuiteproto::InstructionReadFile_ReadFAdvise& proto_fadvise);
};

} // namespace dittosuite
