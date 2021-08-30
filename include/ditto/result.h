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

#pragma once

#include <time.h>

#include <memory>
#include <string>
#include <vector>

namespace dittosuite {

class Result {
 public:
  explicit Result(const std::string& name, const std::vector<timespec>& time_samples);

  void AddSubResult(std::unique_ptr<Result> result);
  void Analyse();
  void Print(const std::string& instruction_path);
  void PrintHistograms(const std::string& instruction_path);
  void PrintStatisticsTable();
  void MakeStatisticsCsv();

 private:
  struct TimeUnit {
    int dividing_factor;  // dividing factor used for transforming the current time
                          // unit (ns) in another one (ex 1000 for microseconds)
    std::string name;
  };
  TimeUnit time_unit_;
  std::string name_;
  std::vector<timespec> time_samples_;
  std::vector<std::unique_ptr<Result>> sub_results_;
  timespec min_, max_, mean_, median_, sd_;
  std::vector<int> ComputeNormalizedFrequencyVector();
  void PrintStatisticsTableContent(const std::string& instruction_path);
  std::string ComputeNextInstructionPath(const std::string& instruction_path);
  void PrintStatisticInCsv(std::ostream& csv_stream, const std::string& instruction_path);
  void MakeHistogramFromVector(const std::vector<int>& freq_vector, const int& min_value);
  TimeUnit GetTimeUnit(const timespec& min_value);
};
}  // namespace dittosuite