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

#include <fcntl.h>
#include <sys/types.h>

#include <ditto/close_file.h>
#include <ditto/delete_file.h>
#include <ditto/instruction_set.h>
#include <ditto/logger.h>
#include <ditto/multithreading.h>
#include <ditto/open_file.h>
#include <ditto/read_directory.h>
#include <ditto/read_write_file.h>
#include <ditto/resize_file.h>
#include <ditto/shared_variables.h>
#include <ditto/syscall.h>

namespace dittosuite {
typedef dittosuiteproto::Instruction::InstructionOneofCase InstructionType;

std::unique_ptr<InstructionSet> InstructionFactory::CreateFromProtoInstructionSet(
    const std::list<int>& thread_ids, const int& repeat,
    const dittosuiteproto::InstructionSet& proto_instruction_set) {
  std::vector<std::unique_ptr<Instruction>> instructions;
  for (const auto& instruction : proto_instruction_set.instructions()) {
    instructions.push_back(
        std::move(InstructionFactory::CreateFromProtoInstruction(thread_ids, instruction)));
  }

  if (proto_instruction_set.has_iterate_options()) {
    const auto& options = proto_instruction_set.iterate_options();

    int list_key = SharedVariables::GetKey(thread_ids, options.list_name());
    int item_key = SharedVariables::GetKey(thread_ids, options.item_name());
    auto type = ConvertAccessType(options.type());
    auto reseeding = ConvertReseeding(options.reseeding());

    uint32_t seed = options.seed();
    if (!options.has_seed()) {
      seed = time(nullptr);
    }

    return std::make_unique<InstructionSet>(Syscall::GetSyscall(), repeat, std::move(instructions),
                                            list_key, item_key, type, reseeding, seed);
  } else {
    return std::make_unique<InstructionSet>(Syscall::GetSyscall(), repeat, std::move(instructions));
  }
}

std::unique_ptr<Instruction> InstructionFactory::CreateFromProtoInstruction(
    const std::list<int>& thread_ids, const dittosuiteproto::Instruction& proto_instruction) {
  int repeat = proto_instruction.repeat();

  switch (proto_instruction.instruction_oneof_case()) {
    case InstructionType::kInstructionSet: {
      return InstructionFactory::CreateFromProtoInstructionSet(thread_ids, repeat,
                                                               proto_instruction.instruction_set());
    }
    case InstructionType::kInstructionOpenFile: {
      const auto& options = proto_instruction.instruction_open_file();

      int fd_key = -1;
      if (options.has_output_fd()) {
        fd_key = SharedVariables::GetKey(thread_ids, options.output_fd());
      }

      if (options.has_input()) {
        int input_key = SharedVariables::GetKey(thread_ids, options.input());
        return std::make_unique<OpenFile>(Syscall::GetSyscall(), repeat, input_key,
                                          options.create(), fd_key);
      } else {
        return std::make_unique<OpenFile>(Syscall::GetSyscall(), repeat, options.path_name(),
                                          options.create(), fd_key);
      }
    }
    case InstructionType::kInstructionDeleteFile: {
      const auto& options = proto_instruction.instruction_delete_file();

      if (options.has_input()) {
        int input_key = SharedVariables::GetKey(thread_ids, options.input());
        return std::make_unique<DeleteFile>(Syscall::GetSyscall(), repeat, input_key);
      } else {
        return std::make_unique<DeleteFile>(Syscall::GetSyscall(), repeat, options.path_name());
      }
    }
    case InstructionType::kInstructionCloseFile: {
      const auto& options = proto_instruction.instruction_close_file();

      int fd_key = SharedVariables::GetKey(thread_ids, options.input_fd());

      return std::make_unique<CloseFile>(Syscall::GetSyscall(), repeat, fd_key);
    }
    case InstructionType::kInstructionResizeFile: {
      const auto& options = proto_instruction.instruction_resize_file();

      int fd_key = SharedVariables::GetKey(thread_ids, options.input_fd());

      return std::make_unique<ResizeFile>(Syscall::GetSyscall(), repeat, options.size(), fd_key);
    }
    case InstructionType::kInstructionWriteFile: {
      const auto& options = proto_instruction.instruction_write_file();

      auto type = ConvertAccessType(options.type());

      uint32_t seed = options.seed();
      if (!options.has_seed()) {
        seed = time(nullptr);
      }

      auto reseeding = ConvertReseeding(options.reseeding());
      int fd_key = SharedVariables::GetKey(thread_ids, options.input_fd());

      return std::make_unique<WriteFile>(Syscall::GetSyscall(), repeat, options.size(),
                                         options.block_size(), options.starting_offset(), type,
                                         seed, reseeding, options.fsync(), fd_key);
    }
    case InstructionType::kInstructionReadFile: {
      const auto& options = proto_instruction.instruction_read_file();

      auto type = ConvertAccessType(options.type());

      uint32_t seed = options.seed();
      if (!options.has_seed()) {
        seed = time(nullptr);
      }

      auto fadvise = ConvertReadFAdvise(type, options.fadvise());
      auto reseeding = ConvertReseeding(options.reseeding());
      int fd_key = SharedVariables::GetKey(thread_ids, options.input_fd());

      return std::make_unique<ReadFile>(Syscall::GetSyscall(), repeat, options.size(),
                                        options.block_size(), options.starting_offset(), type, seed,
                                        reseeding, fadvise, fd_key);
    }
    case InstructionType::kInstructionReadDirectory: {
      const auto& options = proto_instruction.instruction_read_directory();

      int output_key = SharedVariables::GetKey(thread_ids, options.output());

      return std::make_unique<ReadDirectory>(Syscall::GetSyscall(), repeat,
                                             options.directory_name(), output_key);
    }
    case InstructionType::kInstructionResizeFileRandom: {
      const auto& options = proto_instruction.instruction_resize_file_random();

      uint32_t seed = options.seed();
      if (!options.has_seed()) {
        seed = time(nullptr);
      }

      auto reseeding = ConvertReseeding(options.reseeding());
      int fd_key = SharedVariables::GetKey(thread_ids, options.input_fd());

      return std::make_unique<ResizeFileRandom>(Syscall::GetSyscall(), repeat, options.min(),
                                                options.max(), seed, reseeding, fd_key);
    }
    case InstructionType::kMultithreading: {
      const auto& options = proto_instruction.multithreading();

      std::vector<std::unique_ptr<Instruction>> instructions;
      for (const auto& thread : options.threads()) {
        for (int i = 0; i < thread.spawn(); i++) {
          auto thread_ids_copy = thread_ids;
          thread_ids_copy.push_back(InstructionFactory::GenerateThreadId());
          instructions.push_back(std::move(InstructionFactory::CreateFromProtoInstruction(
              thread_ids_copy, thread.instruction())));
        }
      }

      return std::make_unique<Multithreading>(Syscall::GetSyscall(), repeat,
                                              std::move(instructions));
    }
    case InstructionType::INSTRUCTION_ONEOF_NOT_SET: {
      LOGF("Instruction was not set in .ditto file");
    }
    default: {
      LOGF("Invalid instruction was set in .ditto file");
    }
  }
}

int InstructionFactory::GenerateThreadId() {
  return current_thread_id_++;
}

int InstructionFactory::current_thread_id_ = 0;

Reseeding InstructionFactory::ConvertReseeding(const dittosuiteproto::Reseeding& proto_reseeding) {
  switch (proto_reseeding) {
    case dittosuiteproto::Reseeding::ONCE: {
      return kOnce;
    }
    case dittosuiteproto::Reseeding::EACH_ROUND_OF_CYCLES: {
      return kEachRoundOfCycles;
    }
    case dittosuiteproto::Reseeding::EACH_CYCLE: {
      return kEachCycle;
    }
    default: {
      LOGF("Invalid Reseeding was provided");
    }
  }
}

AccessType InstructionFactory::ConvertAccessType(const dittosuiteproto::AccessType& proto_type) {
  switch (proto_type) {
    case dittosuiteproto::AccessType::SEQUENTIAL: {
      return kSequential;
    }
    case dittosuiteproto::AccessType::RANDOM: {
      return kRandom;
    }
    default: {
      LOGF("Invalid AccessType was provided");
    }
  }
}

int InstructionFactory::ConvertReadFAdvise(
    const AccessType& type, const dittosuiteproto::InstructionReadFile_ReadFAdvise& proto_fadvise) {
  switch (proto_fadvise) {
    case dittosuiteproto::InstructionReadFile_ReadFAdvise_AUTOMATIC: {
      switch (type) {
        case kSequential: {
          return POSIX_FADV_SEQUENTIAL;
        }
        case kRandom: {
          return POSIX_FADV_RANDOM;
        }
      }
    }
    case dittosuiteproto::InstructionReadFile_ReadFAdvise_NORMAL: {
      return POSIX_FADV_NORMAL;
    }
    case dittosuiteproto::InstructionReadFile_ReadFAdvise_SEQUENTIAL: {
      return POSIX_FADV_SEQUENTIAL;
    }
    case dittosuiteproto::InstructionReadFile_ReadFAdvise_RANDOM: {
      return POSIX_FADV_RANDOM;
    }
    default: {
      LOGF("Invalid ReadFAdvise was provided");
    }
  }
}

}  // namespace dittosuite
