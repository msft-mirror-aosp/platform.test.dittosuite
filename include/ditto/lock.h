// Copyright (C) 2024 The Android Open Source Project
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

#include <mutex>
#include <stack>

#include <ditto/instruction.h>

namespace dittosuite {

class LockInterface : public Instruction {
 protected:
  pthread_mutex_t* mutex_;

  LockInterface(const LockInterface&) = delete;
  LockInterface& operator=(const LockInterface&) = delete;

  LockInterface(const std::string& name, const Params& params, pthread_mutex_t* mutex);
  virtual ~LockInterface() = default;
};

class Lock : public LockInterface {
 public:
  inline static const std::string kName = "lock";

  Lock(const Lock&) = delete;
  Lock& operator=(const Lock&) = delete;

  explicit Lock(const Params& params, pthread_mutex_t* mutex);
  ~Lock() = default;

 private:
  void SetUpSingle() override;
  void RunSingle() override;
};

class Unlock : public LockInterface {
 public:
  inline static const std::string kName = "unlock";

  Unlock(const Unlock&) = delete;
  Unlock& operator=(const Unlock&) = delete;

  explicit Unlock(const Params& params, pthread_mutex_t* mutex);
  ~Unlock() = default;

 private:
  void SetUpSingle() override;
  void RunSingle() override;
};

}  // namespace dittosuite
