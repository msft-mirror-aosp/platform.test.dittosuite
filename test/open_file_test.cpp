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

#include <gtest/gtest.h>

#include <ditto/open_file.h>
#include <ditto/shared_variables.h>

#ifdef __ANDROID__
const std::string absolute_path = "/data/local/tmp/";
#else
const std::string absolute_path = "";
#endif

class OpenFileTest : public ::testing::Test {
 protected:
  std::string file_name = "test";

  // Set absolute_path
  void SetUp() override {
    auto absolute_path_key = dittosuite::SharedVariables::GetKey("absolute_path");
    dittosuite::SharedVariables::Set(absolute_path_key, absolute_path);
    dittosuite::Instruction::SetAbsolutePathKey(absolute_path_key);
  }
  // Make sure that the files, which have been created in the tests, are deleted
  void TearDown() override { unlink((absolute_path + file_name).c_str()); }
};

TEST_F(OpenFileTest, FileCreated) {
  dittosuite::OpenFile instruction(1, file_name, true, -1);
  instruction.Run();

  ASSERT_EQ(access((absolute_path + file_name).c_str(), F_OK), 0);
}
