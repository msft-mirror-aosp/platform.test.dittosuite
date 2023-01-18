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

#include <ditto/logger.h>
#include <ditto/time_sampler.h>
#include <gtest/gtest.h>

#include <unistd.h>

namespace dittosuite {

TEST(DittoTimer, SimpleTimer) {
  dittosuite::TimeSampler my_time_sampler;
  my_time_sampler.MeasureStart();
  sleep(7);
  my_time_sampler.MeasureEnd();
  std::string outp =
      "Time Difference: " + std::to_string(my_time_sampler.GetTimeSamples().front().tv_sec) + " s, " +
      std::to_string(my_time_sampler.GetTimeSamples().front().tv_sec) + " ns";
  Logger::GetInstance().SetLogLevel(LOG_LEVEL_INFO);
  LOGI(outp);
  outp = "Vector size: " + std::to_string(my_time_sampler.GetTimeSamples().size());
  LOGI(outp);
}

TEST(TimeSamplerTest, DivisionTest) {
  timespec t1 = {2, 6};
  ASSERT_EQ(t1 / NsToTimespec(2), timespec({1, 3}));
}

TEST(TimeSamplerTest, DivisionByZeroTest) {
  timespec t1 = {2, 6};
  EXPECT_DEATH(t1 / NsToTimespec(0), "");
}

}  // namespace dittosuite
