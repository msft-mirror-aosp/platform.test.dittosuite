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

namespace dittosuite {

class Instruction {
 public:
  explicit Instruction(int repeat);
  virtual ~Instruction() = default;

  virtual void SetUp() = 0;
  void Run();
  virtual void TearDown() = 0;

  static void SetAbsolutePath(const std::string& absolute_path);
  static std::string GetAbsolutePath();

 private:
  virtual void RunSingle() = 0;

  static std::string absolute_path_;
  int repeat_;
};

} // namespace dittosuite
