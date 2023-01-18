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
#include <ditto/sampler.h>
#include <gtest/gtest.h>

#include <unistd.h>

namespace dittosuite {

std::vector<timespec> tss = {{0, 0},         {0, 1}, {1, 0}, {1, 1},
                             {1, 999999999}, {2, 2}, {2, 3}, {3, 1}};
std::vector<int64_t> tss_ns = {0,          1,          1000000000, 1000000001,
                               1999999999, 2000000002, 2000000003, 3000000001};

TEST(TimespecUtilsTest, TimespecToNs) {
  for (unsigned int i = 0; i < tss.size(); ++i) {
    ASSERT_EQ(TimespecToNs(tss[i]), tss_ns[i]);
  }
}

TEST(TimespecUtilsTest, NsToTimespec) {
  for (unsigned int i = 0; i < tss.size(); ++i) {
    ASSERT_EQ(tss[i], NsToTimespec(tss_ns[i]));
  }
}

TEST(TimespecUtilsTest, TimespecToNsInverse) {
  for (const auto& ts0 : tss) {
    ASSERT_EQ(ts0, NsToTimespec(TimespecToNs(ts0)));
  }
}

TEST(TimespecUtilsTest, EqualityOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      ASSERT_EQ(ts0 == ts1, ts0_ns == ts1_ns);
    }
  }
}

TEST(TimespecUtilsTest, DisequalityOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      ASSERT_EQ(ts0 != ts1, ts0_ns != ts1_ns);
    }
  }
}

TEST(TimespecUtilsTest, LessThanOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      ASSERT_EQ(ts0 < ts1, ts0_ns < ts1_ns);
    }
  }
}

TEST(TimespecUtilsTest, LessThanOrEqualOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      ASSERT_EQ(ts0 <= ts1, ts0_ns <= ts1_ns);
    }
  }
}

TEST(TimespecUtilsTest, GreaterThanOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      ASSERT_EQ(ts0 > ts1, ts0_ns > ts1_ns);
    }
  }
}

TEST(TimespecUtilsTest, GreaterThanOrEqualOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      ASSERT_EQ(ts0 >= ts1, ts0_ns >= ts1_ns);
    }
  }
}

TEST(TimespecUtilsTest, SumOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);
      auto sum_ns = ts0_ns + ts1_ns;
      auto sum_ts = ts0 + ts1;

      ASSERT_EQ(TimespecToNs(sum_ts), sum_ns);
      ASSERT_EQ(sum_ts, NsToTimespec(sum_ns));
    }
  }
}

TEST(TimespecUtilsTest, SubtractOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);
      auto sub_ns = ts0_ns - ts1_ns;
      timespec sub_ts;

      if (ts0 < ts1) {
        EXPECT_DEATH(ts0 - ts1, "");
      } else {
        sub_ts = ts0 - ts1;

        ASSERT_EQ(TimespecToNs(sub_ts), sub_ns);
        ASSERT_EQ(sub_ts, NsToTimespec(sub_ns));
      }
    }
  }
}

TEST(TimespecUtilsTest, DivideOperator) {
  for (const auto& ts0 : tss) {
    for (const auto& ts1 : tss) {
      auto ts0_ns = TimespecToNs(ts0);
      auto ts1_ns = TimespecToNs(ts1);

      if (ts1_ns == 0) {
        EXPECT_DEATH(ts0 / ts1, "");
      } else {
        auto div_ns = ts0_ns / ts1_ns;
        timespec div_ts;
        div_ts = ts0 / ts1;

        ASSERT_EQ(TimespecToNs(div_ts), div_ns);
        ASSERT_EQ(div_ts, NsToTimespec(div_ns));
      }
    }
  }
}

}  // namespace dittosuite
