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

#include <ditto/result.h>
#include <ditto/statistics.h>

namespace dittosuite {

Result::Result(const std::string& name, std::vector<timespec> time_samples)
    : name_(name), time_samples_(time_samples) {}

std::string Result::GetName() {
  return name_;
}
std::vector<timespec> Result::GetTimeSamples() {
  return time_samples_;
}

timespec Result::GetMin() const {
  return min_;
}

timespec Result::GetMax() const {
  return max_;
}

timespec Result::GetMean() const {
  return mean_;
}

void Result::AddSubResult(std::unique_ptr<Result> result) {
  sub_results_.push_back(std::move(result));
}

void Result::Analyse() {
  min_ = StatisticsGetMin(time_samples_);
  max_ = StatisticsGetMax(time_samples_);
  mean_ = StatisticsGetMean(time_samples_);
}

}  // namespace dittosuite
