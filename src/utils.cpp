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

#include <ditto/utils.h>

#include <sys/stat.h>
#include <unistd.h>

namespace dittosuite {

int64_t GetFileSize(int fd) {
  struct stat64 sb;
  fstat64(fd, &sb);
  return sb.st_size;
}

}  // namespace dittosuite