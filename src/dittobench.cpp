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

#include <string>

#include <ditto/parser.h>

int main() {
  std::string file = "create_file.ditto";
  auto instruction_set = dittosuite::Parser::Parse(file);

  instruction_set->SetUp();
  instruction_set->Run();
  instruction_set->TearDown();
  return 0;
}
