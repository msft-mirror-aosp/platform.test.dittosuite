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

#include <fstream>

#include <ditto/instruction_factory.h>
#include <ditto/shared_variables.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "schema/benchmark.pb.h"

namespace dittosuite {

Parser& Parser::GetParser() {
  static Parser parser;
  return parser;
}

std::unique_ptr<Instruction> Parser::Parse() {
  std::unique_ptr<dittosuiteproto::Benchmark> benchmark =
      std::make_unique<dittosuiteproto::Benchmark>();
  google::protobuf::io::FileInputStream file_input(open(file_path_.c_str(), O_CLOEXEC));
  google::protobuf::TextFormat::Parse(&file_input, benchmark.get());

  auto absolute_path_key = SharedVariables::GetKey("absolute_path");
  SharedVariables::Set(absolute_path_key, benchmark->global().absolute_path());
  Instruction::SetAbsolutePathKey(absolute_path_key);

  auto instructions = InstructionFactory::CreateFromProtoInstruction(benchmark->main());
  SharedVariables::ClearKeys();

  return instructions;
}

void Parser::SetFilePath(const std::string& file_path) {
  file_path_ = file_path;
}

} // namespace dittosuite

