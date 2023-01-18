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

#include <ditto/statistics.h>
#include <ditto/time_sampler.h>

#include <cmath>

#include <algorithm>

namespace dittosuite {

int64_t TimespecToNs(const timespec& t) {
  return t.tv_sec * 1e9 + t.tv_nsec;
}

timespec NsToTimespec(const int64_t& t) {
  timespec result;
  result.tv_sec = t / 1e9;
  result.tv_nsec = t % static_cast<int64_t>(1e9);
  return result;
}

timespec StatisticsGetMin(const std::vector<timespec>& time_samples) {
  return *std::min_element(time_samples.begin(), time_samples.end(), [](timespec t1, timespec t2) {
    return ((t1.tv_sec < t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_nsec <= t2.tv_nsec));
  });
}

timespec StatisticsGetMax(const std::vector<timespec>& time_samples) {
  return *std::min_element(time_samples.begin(), time_samples.end(), [](timespec t1, timespec t2) {
    return ((t1.tv_sec > t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_nsec >= t2.tv_nsec));
  });
}

timespec StatisticsGetMean(const std::vector<timespec>& time_samples) {
  int64_t result = 0;
  for (const auto& time_sample : time_samples) result += TimespecToNs(time_sample);
  return NsToTimespec(result / time_samples.size());
}

bool CompareTimespec(timespec t1, timespec t2) {
  return ((t1.tv_sec < t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_nsec <= t2.tv_nsec));
}

timespec StatisticsGetMedian(const std::vector<timespec>& time_samples) {
  std::vector<timespec> time_samples_copy = time_samples;
  int n = time_samples.size();
  if (n % 2) {
    // odd number of elements, the median is the element in the middle
    std::nth_element(time_samples_copy.begin(), time_samples_copy.begin() + n / 2,
                     time_samples_copy.end(), CompareTimespec);
    return time_samples_copy[n / 2];
  } else {
    // even number of elements, the median is the average between the two middle elements
    std::nth_element(time_samples_copy.begin(), time_samples_copy.begin() + n / 2,
                     time_samples_copy.end(), CompareTimespec);
    std::nth_element(time_samples_copy.begin(), time_samples_copy.begin() + (n - 1) / 2,
                     time_samples_copy.end(), CompareTimespec);
    int64_t result =
        (TimespecToNs(time_samples_copy[n / 2]) + TimespecToNs(time_samples_copy[(n - 1) / 2])) / 2;
    return NsToTimespec(result);
  }
}

// TODO(lucialup): improve readability
// the standard deviation is computed as the square root of the
// variance given by the deviation of each point relative to the mean
// SD = sqrt( variance ),
// where variance = Sum( (point_i - mean)^2 ) / total_number_of_points
timespec StatisticsGetSd(const std::vector<timespec>& time_samples) {
  int64_t mean = TimespecToNs(StatisticsGetMean(time_samples));
  int64_t result(0), deviation_pow;
  int64_t deviation;
  for (const auto& time_sample : time_samples) {
    deviation = TimespecToNs(time_sample) - mean;
    deviation_pow = std::pow(deviation, 2);
    result += deviation_pow;  // TODO(lucialup): add overflow error handling
  }
  int time_samples_size = time_samples.size();
  return NsToTimespec(std::sqrt(result / time_samples_size));
}
}  // namespace dittosuite