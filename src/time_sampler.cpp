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

#include <ditto/time_sampler.h>

#include <time.h>

#include <ditto/logger.h>

namespace dittosuite {

bool IsTimeSampleValid(const timespec& start, const timespec& end) {
  return (end.tv_sec > start.tv_sec) || (end.tv_sec == start.tv_sec && end.tv_nsec > start.tv_nsec);
}

timespec subtract(const timespec& t1, const timespec& t2) {
  timespec result = {0, 0};
  if (IsTimeSampleValid(t2, t1)) {
    result.tv_sec = t1.tv_sec - t2.tv_sec;
    if (t1.tv_nsec < t2.tv_nsec) {
      result.tv_sec--;
      result.tv_nsec = 1e9 - t2.tv_nsec + t1.tv_nsec;
    } else
      result.tv_nsec = t1.tv_nsec - t2.tv_nsec;
    return result;
  }
  LOGE("At timer, end time is smaller than start time");
  return result;
}

std::vector<timespec> TimeSampler::GetTimeSamples() const {
  return time_samples_;
}

void TimeSampler::AddTimeSample(timespec sample) {
  time_samples_.push_back(sample);
}

void TimeSampler::MeasureStart() {
  clock_gettime(CLOCK_MONOTONIC, &start_);
}

void TimeSampler::MeasureEnd() {
  clock_gettime(CLOCK_MONOTONIC, &end_);
  AddTimeSample(subtract(end_, start_));
}

}  // namespace dittosuite