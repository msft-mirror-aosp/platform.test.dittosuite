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

#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>

#include <gtest/gtest.h>

#include <ditto/open_file.h>
#include <ditto/resize_file.h>
#include <ditto/shared_variables.h>

TEST(ResizeFileTest, ResizeFileTestRun) {
  int repeat = 1;
  std::string file = "/data/local/tmp/newfile.txt";
  int64_t size = 2048;

  dittosuite::SharedVariables::Set("absolute_path", "");

  int fd_key = dittosuite::SharedVariables::GetKey("test_file");

  dittosuite::OpenFile open_file_instruction(repeat, file, true);
  open_file_instruction.SetOutputFdKey(fd_key);
  open_file_instruction.Run();

  ASSERT_EQ(access(file.c_str(), F_OK), 0);

  dittosuite::ResizeFile resize_file_instruction(repeat, size);
  resize_file_instruction.SetInputFdKey(fd_key);
  resize_file_instruction.Run();

  struct stat sb;
  stat(file.c_str(), &sb);
  ASSERT_EQ(sb.st_size, size);
}
