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

#pragma once

#if __ANDROID__

#include <cstdint>
#include <random>

#include <ditto/instruction.h>

#include <ditto/binder.h>

namespace dittosuite {

class BinderRequest : public Instruction {
 public:
  inline static const std::string kName = "binder_request";

  explicit BinderRequest(SyscallInterface& syscall, int repeat, const std::string& service_name);

 protected:
  void RunSingle() override;

 private:
  std::string service_name_;
  android::sp<IDittoBinder> service_;

  void SetUp() override;
  void TearDownSingle(bool is_last) override;
};

}  // namespace dittosuite

#endif
