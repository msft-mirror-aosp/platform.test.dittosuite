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

#include <sys/types.h>

#include <ditto/close_file.h>
#include <ditto/delete_file.h>
#include <ditto/instruction_set.h>
#include <ditto/logger.h>
#include <ditto/open_file.h>
#include <ditto/resize_file.h>
#include <ditto/shared_variables.h>
#include <ditto/write_file.h>

namespace dittosuite {
typedef dittosuiteproto::Instruction::InstructionOneofCase InstructionType;

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
  int repeat = proto_instruction.repeat();

  switch (proto_instruction.instruction_oneof_case()) {
    case InstructionType::kInstructionSet: {
      return InstructionFactory::CreateFromProtoInstructionSet(repeat,
                                                               proto_instruction.instruction_set());
    }
    case InstructionType::kInstructionOpenFile: {
      const auto& options = proto_instruction.instruction_open_file();

      int fd_key = -1;
      if (options.has_output_fd()) {
        fd_key = SharedVariables::GetKey(options.output_fd());
      }

      auto instruction =
          std::make_unique<OpenFile>(repeat, options.file(), options.create(), fd_key);

      return instruction;
    }
    case InstructionType::kInstructionDeleteFile: {
      const auto& options = proto_instruction.instruction_delete_file();

      return std::make_unique<DeleteFile>(repeat, options.file());
    }
    case InstructionType::kInstructionCloseFile: {
      const auto& options = proto_instruction.instruction_close_file();

      int fd_key = SharedVariables::GetKey(options.input_fd());
      auto instruction = std::make_unique<CloseFile>(repeat, fd_key);

      return instruction;
    }
    case InstructionType::kInstructionResizeFile: {
      const auto& options = proto_instruction.instruction_resize_file();

      int fd_key = SharedVariables::GetKey(options.input_fd());
      auto instruction = std::make_unique<ResizeFile>(repeat, options.size(), fd_key);

      return instruction;
    }
    case InstructionType::kInstructionWriteFile: {
      const auto& options = proto_instruction.instruction_write_file();

      ReadWriteType type;
      switch (options.type()) {
        case dittosuiteproto::ReadWriteType::sequential: {
          type = ReadWriteType::kSequential;
          break;
        }
        case dittosuiteproto::ReadWriteType::random: {
          type = ReadWriteType::kRandom;
          break;
        }
        default: {
          LOGE("Invalid ReadWriteType was provided");
          return nullptr;
        }
      }

      u_int32_t seed = options.seed();
      if (!options.has_seed()) {
        seed = time(0);
      }

      int fd_key = SharedVariables::GetKey(options.input_fd());
      auto instruction = std::make_unique<WriteFile>(repeat, options.size(), options.block_size(),
                                                     type, seed, fd_key);

      return instruction;
    }
    case InstructionType::INSTRUCTION_ONEOF_NOT_SET: {
      LOGE("Instruction was not set in .ditto file");
      return nullptr;
    }
    default: {
      LOGE("Invalid instruction was set in .ditto file");
      return nullptr;
    }
  }
}

} // namespace dittosuite

