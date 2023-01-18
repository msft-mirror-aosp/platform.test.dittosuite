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

#include <fcntl.h>

#include <cstdlib>
#include <fstream>

#include <ditto/instruction_factory.h>
#include <ditto/logger.h>
#include <ditto/shared_variables.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#ifdef __ANDROID__
#include "test/dittosuite/schema/benchmark.pb.h"
#else
#include "schema/benchmark.pb.h"
#endif

namespace dittosuite {

Parser& Parser::GetParser() {
  static Parser parser;
  return parser;
}

void Parser::Parse(const std::string& file_path) {
  std::unique_ptr<dittosuiteproto::Benchmark> benchmark =
      std::make_unique<dittosuiteproto::Benchmark>();

  int fd = open(file_path.c_str(), O_CLOEXEC);
  if (fd == -1) {
    LOGF("Provided .ditto file was not found");
  }

  google::protobuf::io::FileInputStream file_input(fd);
  if (!google::protobuf::TextFormat::Parse(&file_input, benchmark.get())) {
    LOGF("Error while parsing .ditto file");
  }

  std::list<int> thread_ids({InstructionFactory::GenerateThreadId()});
  auto absolute_path_key = SharedVariables::GetKey(thread_ids, "absolute_path");
  SharedVariables::Set(absolute_path_key, benchmark->global().absolute_path());
  Instruction::SetAbsolutePathKey(absolute_path_key);

  if (benchmark->has_init()) {
    init_ = InstructionFactory::CreateFromProtoInstruction(thread_ids, benchmark->init());
  }
  main_ = InstructionFactory::CreateFromProtoInstruction(thread_ids, benchmark->main());
  if (benchmark->has_clean_up()) {
    clean_up_ = InstructionFactory::CreateFromProtoInstruction(thread_ids, benchmark->clean_up());
  }

  SharedVariables::ClearKeys();
}

std::unique_ptr<Instruction> Parser::GetInit() {
  return std::move(init_);
}

std::unique_ptr<Instruction> Parser::GetMain() {
  return std::move(main_);
}

std::unique_ptr<Instruction> Parser::GetCleanUp() {
  return std::move(clean_up_);
}

}  // namespace dittosuite
