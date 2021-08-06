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

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <gtest/gtest.h>

#include <ditto/read_directory.h>
#include <ditto/shared_variables.h>

#ifdef __ANDROID__
const std::string absolute_path = "/data/local/tmp/";
#else
const std::string absolute_path = "";
#endif

class ReadDirectoryTest : public ::testing::Test {
 protected:
  std::string directory_name = "test_directory";
  std::vector<std::string> file_names{"test1", "test2", "test3"};

  // Create folder with several files for testing and set absolute_path
  void SetUp() override {
    auto absolute_path_key = dittosuite::SharedVariables::GetKey("absolute_path");
    dittosuite::SharedVariables::Set(absolute_path_key, absolute_path);
    dittosuite::Instruction::SetAbsolutePathKey(absolute_path_key);

    ASSERT_NE(mkdir(directory_name.c_str(), S_IRWXU), -1);
    for (const auto& file_name : file_names) {
      ASSERT_NE(
          open((directory_name + "/" + file_name).c_str(), O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR),
          -1);
    }
  }
  // Remove the folder and files that were created in SetUp()
  void TearDown() override {
    for (const auto& file_name : file_names) {
      ASSERT_NE(unlink((directory_name + "/" + file_name).c_str()), -1);
    }
    ASSERT_NE(rmdir(directory_name.c_str()), -1);
  }
};

TEST_F(ReadDirectoryTest, ReadDirectoryTestRun) {
  auto output_key = dittosuite::SharedVariables::GetKey("file_list");

  dittosuite::ReadDirectory instruction(1, directory_name, output_key);
  instruction.Run();

  auto output = std::get<std::vector<std::string>>(dittosuite::SharedVariables::Get(output_key));
  sort(output.begin(), output.end());

  ASSERT_EQ(output, file_names);
}
