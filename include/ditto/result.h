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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dittosuite {

class Result {
 public:
  explicit Result(const std::string& name);

  void AddMeasurement(const std::string& type, const std::vector<int64_t>& samples);
  void AddSubResult(std::unique_ptr<Result> result);
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
  struct Statistics {
    int64_t min, max, mean, median;
    double sd;
  };
  TimeUnit time_unit_;
  std::string name_;
  std::map<std::string, std::vector<int64_t>> samples_;
  std::map<std::string, Statistics> statistics_;
  std::vector<std::unique_ptr<Result>> sub_results_;

  void AnalyseMeasurement(const std::string& name);
  void PrintMeasurement(const std::string& name);
  std::vector<int> ComputeNormalizedFrequencyVector(const std::string& measurement_name);
  void PrintStatisticsTableContent(const std::string& instruction_path,
                                   const std::string& measurement_name);

  std::string ComputeNextInstructionPath(const std::string& instruction_path);
  void PrintStatisticInCsv(std::ostream& csv_stream, const std::string& instruction_path,
                           const std::string& measurement_name);
  void PrintHistogramHeader(const std::string& measurement_name);
  void MakeHistogramFromVector(const std::vector<int>& freq_vector, const int& min_value);
  TimeUnit GetTimeUnit(const int64_t& min_value);
  void PrintMeasurementStatisticInCsv(std::ostream& csv_stream, const std::string& name);
};
}  // namespace dittosuite