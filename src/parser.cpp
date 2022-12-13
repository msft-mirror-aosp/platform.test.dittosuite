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

#include <ditto/parser.h>
#include <ditto/instruction_factory.h>
#include <fcntl.h>

#include <fstream>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "test/dittosuite/schema/benchmark.pb.h"

namespace dittosuite {

  std::queue<std::unique_ptr<Instruction> > Parser::Parse(std::string& file_name) {
    std::queue<std::unique_ptr<Instruction> > instruction_queue;

    dittosuiteproto::Benchmark* benchmark = new dittosuiteproto::Benchmark;
    google::protobuf::io::FileInputStream file_input(open(file_name.c_str(), O_RDONLY));
    google::protobuf::TextFormat::Parse(&file_input, benchmark);

    // TODO(robertasn): handle Global directive

    for (const auto& instruction : benchmark->main().instructions()) {
      instruction_queue.push(InstructionFactory::CreateFromProtoInstruction(instruction));
    }

    return instruction_queue;
  }

} // namespace dittosuite

