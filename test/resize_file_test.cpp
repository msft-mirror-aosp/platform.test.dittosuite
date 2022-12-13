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
#include <ditto/syscall.h>

#ifdef __ANDROID__
const std::string absolute_path = "/data/local/tmp/";
#else
const std::string absolute_path = "";
#endif

TEST(ResizeFileTest, ResizeFileTestRun) {
  int repeat = 1;
  std::string file = "newfile.txt";
  int64_t size = 2048;

  auto absolute_path_key = dittosuite::SharedVariables::GetKey("absolute_path");
  dittosuite::SharedVariables::Set(absolute_path_key, absolute_path);
  dittosuite::Instruction::SetAbsolutePathKey(absolute_path_key);

  int fd_key = dittosuite::SharedVariables::GetKey("test_file");

  dittosuite::OpenFile open_file_instruction(dittosuite::Syscall::GetSyscall(), repeat, file, true,
                                             fd_key);
  open_file_instruction.Run();

  ASSERT_EQ(access((absolute_path + file).c_str(), F_OK), 0);

  dittosuite::ResizeFile resize_file_instruction(dittosuite::Syscall::GetSyscall(), repeat, size,
                                                 fd_key);
  resize_file_instruction.Run();

  struct stat sb;
  stat((absolute_path + file).c_str(), &sb);
  ASSERT_EQ(sb.st_size, size);
}
