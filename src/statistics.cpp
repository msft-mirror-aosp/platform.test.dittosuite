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

timespec StatisticsGetMedian(const std::vector<timespec>& time_samples) {
  std::vector<timespec> time_samples_copy = time_samples;
  int n = time_samples.size();
  if (n % 2) {
    // odd number of elements, the median is the element in the middle
    std::nth_element(time_samples_copy.begin(), time_samples_copy.begin() + n / 2,
                     time_samples_copy.end());
    return time_samples_copy[n / 2];
  } else {
    // even number of elements, the median is the average between the two middle elements
    std::nth_element(time_samples_copy.begin(), time_samples_copy.begin() + n / 2,
                     time_samples_copy.end());
    std::nth_element(time_samples_copy.begin(), time_samples_copy.begin() + (n - 1) / 2,
                     time_samples_copy.end());
    int64_t result =
        (TimespecToNs(time_samples_copy[n / 2]) + TimespecToNs(time_samples_copy[(n - 1) / 2])) / 2;
    return NsToTimespec(result);
  }
}

// The standard deviation sd of a population of N samples, where x_i is the
// i-th sample and x is the average among all the samples is computed as:
//
// sd = sqrt( sum( (x_i - x)^2 ) / N )
double StatisticsGetSd(const std::vector<timespec>& samples) {
  double mean = TimespecToNs(StatisticsGetMean(samples));
  double variance;

  variance = 0.0;
  for (const auto& s : samples) {
    double deviation = TimespecToNs(s) - mean;
    double deviation_square = std::pow(deviation, 2);
    variance += deviation_square;  // TODO(lucialup): add overflow error handling
  }
  variance /= samples.size();

  return std::sqrt(variance);
}

}  // namespace dittosuite
