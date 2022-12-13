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

TEST(OpenFileTest, OpenFileTestRun) {
  int repeat = 1;
  std::string file = "/data/local/tmp/newfile.txt";

  dittosuite::SharedVariables::Set("absolute_path", "");

  dittosuite::OpenFile open_file_instruction(repeat, file, true);
  open_file_instruction.Run();

  ASSERT_EQ(access(file.c_str(), F_OK), 0);
}
