// Copyright (C) 2023 The Android Open Source Project
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

#if __ANDROID__

#include <ditto/binder_request.h>

#include <ditto/binder.h>
#include <ditto/logger.h>

namespace dittosuite {

BinderRequest::BinderRequest(SyscallInterface& syscall, int repeat, const std::string& service_name)
    : Instruction(syscall, kName, repeat), service_name_(service_name) {}

void BinderRequest::RunSingle() {
  const char c = 1;

  char ret = service_->sync(c);
  if (ret != (~c)) {
    LOGF("Wrong result, expected: " + std::to_string(~c) +
         ", but got: " + std::to_string(ret));
  }
}

void BinderRequest::SetUp() {
  LOGD("Starting binder requester for service: " + service_name_);
  service_ = getBinderService(service_name_);
  service_->start();
  Instruction::SetUp();
}

void BinderRequest::TearDownSingle(bool is_last) {
  Instruction::TearDownSingle(is_last);
  if (is_last) {
    LOGD("This is the last, sending termination request");
    service_->end();
  }
}

}  // namespace dittosuite

#endif
