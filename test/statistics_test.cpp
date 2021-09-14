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
#include <gtest/gtest.h>

#include <vector>

namespace dittosuite {
std::vector<int64_t> vec1 = {1, 5, 3, 4, 2, 6};
std::vector<int64_t> vec2 = {1, 1, 5};
std::vector<int64_t> vec3 = {3, 3, 3, 3};
std::vector<int64_t> vec4 = {};

TEST(StatisticsTest, StatisticsMinTest) {
  ASSERT_EQ(1, StatisticsGetMin(vec1));
  ASSERT_EQ(1, StatisticsGetMin(vec2));
  ASSERT_EQ(3, StatisticsGetMin(vec3));
  EXPECT_DEATH(StatisticsGetMin(vec4), "");
}

TEST(StatisticsTest, StatisticsMaxTest) {
  ASSERT_EQ(6, StatisticsGetMax(vec1));
  ASSERT_EQ(5, StatisticsGetMax(vec2));
  ASSERT_EQ(3, StatisticsGetMax(vec3));
  EXPECT_DEATH(StatisticsGetMax(vec4), "");
}

TEST(StatisticsTest, StatisticsMeanTest) {
  ASSERT_EQ(3, StatisticsGetMean(vec1));
  ASSERT_EQ(2, StatisticsGetMean(vec2));
  ASSERT_EQ(3, StatisticsGetMean(vec3));
  EXPECT_DEATH(StatisticsGetMean(vec4), "");
}

TEST(StatisticsTest, StatisticsMedianTest) {
  ASSERT_EQ(3, StatisticsGetMedian(vec1));
  ASSERT_EQ(1, StatisticsGetMedian(vec2));
  ASSERT_EQ(3, StatisticsGetMedian(vec3));
  EXPECT_DEATH(StatisticsGetMedian(vec4), "");
}

TEST(StatisticsTest, StatisticsSdTest) {
  ASSERT_FLOAT_EQ(1.77951304, StatisticsGetSd(vec1));
  ASSERT_FLOAT_EQ(1.914854215, StatisticsGetSd(vec2));
  ASSERT_EQ(0, StatisticsGetSd(vec3));
  EXPECT_DEATH(StatisticsGetSd(vec4), "");
}

}  // namespace dittosuite