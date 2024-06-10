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

#include <ditto/embedded_benchmarks.h>
#include <ditto/instruction_factory.h>
#include <ditto/logger.h>
#include <ditto/shared_variables.h>

#include <google/protobuf/text_format.h>

namespace dittosuite {

Parser& Parser::GetParser() {
  static Parser parser;
  return parser;
}

std::unique_ptr<dittosuiteproto::Benchmark> Parser::__Parse(
    std::string json_benchmark, const std::vector<std::string>& parameters)

{
  std::unique_ptr<dittosuiteproto::Benchmark> benchmark =
      std::make_unique<dittosuiteproto::Benchmark>();

  for (std::size_t i = 0; i < parameters.size(); i++) {
    std::string to_replace("$PARAMETER_" + std::to_string(i + 1) + "$");
    auto position = json_benchmark.find(to_replace);
    if (position == std::string::npos) {
      LOGW(to_replace + " does not exist in .ditto file");
      continue;
    }
    json_benchmark.replace(position, to_replace.size(), parameters[i]);
  }

  if (!google::protobuf::TextFormat::ParseFromString(json_benchmark, benchmark.get())) {
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

  return benchmark;
}

std::unique_ptr<dittosuiteproto::Benchmark> Parser::ParseEmbedded(
    const std::string& embedded_benchmark, const std::vector<std::string>& parameters) {
  auto json_benchmark_it = ditto_static_config.find(embedded_benchmark);
  if (json_benchmark_it == ditto_static_config.end()) {
    LOGF("The requested benchmark is invalid: " + embedded_benchmark);
  }

  return __Parse(json_benchmark_it->second, parameters);
}

std::unique_ptr<dittosuiteproto::Benchmark> Parser::ParseFile(
    const std::string& file_path, const std::vector<std::string>& parameters) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    LOGF("Provided .ditto file was not found: " + file_path);
  }

  std::string json_benchmark((std::istreambuf_iterator<char>(file)),
                             (std::istreambuf_iterator<char>()));

  return __Parse(json_benchmark, parameters);
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
