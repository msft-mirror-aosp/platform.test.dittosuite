// Copyright 2021 Google LLC
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

#include <vector>

#include <benchmark/benchmark.h>

auto BM_test = [](benchmark::State& st, auto Inputs) {
  std::vector<decltype(Inputs)> in;
  while (st.KeepRunning()) {
    in.push_back(Inputs);
    benchmark::ClobberMemory();
  }
};

int main(int argc, char** argv) {
  for (const auto& test_input : {"asdf", "asdfasdfasdfasdfasdf", "asdfasdfasdfasdfasdfasdfasdf",
                                 "asdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf", "asda"}) {
    benchmark::RegisterBenchmark(test_input, BM_test, test_input);
  }
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();

  return 0;
}