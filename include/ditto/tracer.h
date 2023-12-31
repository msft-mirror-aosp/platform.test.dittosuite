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

#include <memory>
#include <fstream>

#ifdef __ANDROID__
#include <benchmark.pb.h>
#else
#include "schema/benchmark.pb.h"
#endif


namespace dittosuite {

class Tracer {
 public:
  Tracer();
  ~Tracer();
  Tracer(Tracer& other) = delete;

  void StartSession(std::unique_ptr<dittosuiteproto::Benchmark> benchmark);
  void Start(const std::string &splice);
  void End(const std::string &splice);

 private:
  std::ofstream trace_marker_;
  std::string id_;
};

template <class T>
std::string trace_format__(const T& id) {
  return std::string("|") + id;
}

template <class T, class... Args>
std::string trace_format__(const T& id, Args... vars) {
  return std::string("|") + id + trace_format__(vars...);
}

template <class T>
std::string trace_format(const T& id) {
  return id;
}

template <class T, class... Args>
std::string trace_format(const T& id, Args... vars) {
  std::string result;

  result = trace_format(id);

  return result + trace_format__(vars...);
}


}  // namespace dittosuite
