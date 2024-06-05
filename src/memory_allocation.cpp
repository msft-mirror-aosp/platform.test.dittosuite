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

#include <ditto/memory_allocation.h>

#include <ditto/logger.h>

#include <sys/mman.h>

namespace dittosuite {

void allocate_memory(std::stack<void*>* addresses, size_t size) {
  const int protection = PROT_READ | PROT_WRITE;
  const int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED;
  void* addr = nullptr;

  addr = mmap(NULL, size, protection, flags, -1, 0);
  if (addr == MAP_FAILED) {
    PLOGF("mmap failed");
  }

  addresses->push(addr);
  LOGD("mmap successful, stack size: " + std::to_string(addresses->size()));
}

void deallocate_memory(std::stack<void*>* addresses, size_t size) {
  while (!addresses->empty()) {
    void* addr = addresses->top();
    addresses->pop();
    if (munmap(addr, size) == -1) {
      PLOGF("munmap failed");
    }
    LOGD("munmap successful, stack size: " + std::to_string(addresses->size()));
  }
}

MemoryAllocation::MemoryAllocation(const Params& params, const uint64_t size,
                                   const FreePolicy free_policy)
    : Instruction(kName, params), size_(size), free_policy_(free_policy) {}

MemoryAllocation::~MemoryAllocation() {
  deallocate_memory(&allocated_addresses_, size_);
}

void MemoryAllocation::RunSingle() {
  allocate_memory(&allocated_addresses_, size_);
}

void MemoryAllocation::TearDownSingle(bool is_last) {
  switch (free_policy_) {
    case dittosuite::FreePolicy::kKeep:
      break;
    case dittosuite::FreePolicy::kFreeEveryPeriod:
      deallocate_memory(&allocated_addresses_, size_);
      break;
    case dittosuite::FreePolicy::kFreeLastPeriod:
      if (is_last) {
        deallocate_memory(&allocated_addresses_, size_);
      }
      break;
  }
}

}  // namespace dittosuite
