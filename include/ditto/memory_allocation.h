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

#include <ditto/instruction.h>

namespace dittosuite {

class MemoryAllocation : public Instruction {
 public:
  inline static const std::string kName = "memory_allocation";

  explicit MemoryAllocation(const Params& params, const uint64_t size);
  ~MemoryAllocation();

 private:
  size_t size_;
  char* allocated_memory_;

  void RunSingle();
};

}  // namespace dittosuite
