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

#include <ctime>
#include <vector>

namespace dittosuite {

bool operator<(const timespec& t1, const timespec& t2);
bool operator<=(const timespec& t1, const timespec& t2);
bool operator>(const timespec& t1, const timespec& t2);
bool operator>=(const timespec& t1, const timespec& t2);
timespec operator-(const timespec& t1, const timespec& t2);
timespec operator+(const timespec& t1, const timespec& t2);

class TimeSampler {
 public:
  std::vector<timespec> GetTimeSamples() const;
  void AddTimeSample(timespec sample);
  void MeasureStart();
  void MeasureEnd();

 private:
  timespec start_;
  timespec end_;
  std::vector<timespec> time_samples_;
};
}  // namespace dittosuite
