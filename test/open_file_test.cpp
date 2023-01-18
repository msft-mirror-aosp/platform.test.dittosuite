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

#include <unistd.h>

#include <gtest/gtest.h>

#include <ditto/open_file.h>
#include <ditto/shared_variables.h>

#ifdef __ANDROID__
const std::string absolute_path = "/data/local/tmp/";
#else
const std::string absolute_path = "";
#endif

TEST(OpenFileTest, OpenFileTestRun) {
  int repeat = 1;
  std::string file = "newfile.txt";

  auto absolute_path_key = dittosuite::SharedVariables::GetKey("absolute_path");
  dittosuite::SharedVariables::Set(absolute_path_key, absolute_path);
  dittosuite::Instruction::SetAbsolutePathKey(absolute_path_key);

  dittosuite::OpenFile open_file_instruction(repeat, file, true, -1);
  open_file_instruction.Run();

  ASSERT_EQ(access((absolute_path + file).c_str(), F_OK), 0);
}
