// Copyright (C) 2023 The Android Open Source Project
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

#include <ditto/multithreading_utils.h>

namespace dittosuite {

std::string to_string(const SchedAttr__& attr) {
  std::string ret;

  ret += "size: " + std::to_string(attr.size);
  ret += ", policy: " + std::to_string(attr.sched_policy);
  ret += ", flags: " + std::to_string(attr.sched_flags);
  ret += ", nice: " + std::to_string(attr.sched_nice);
  ret += ", priority: " + std::to_string(attr.sched_priority);
  ret += ", runtime: " + std::to_string(attr.sched_runtime);
  ret += ", deadline: " + std::to_string(attr.sched_deadline);
  ret += ", period: " + std::to_string(attr.sched_period);

  return ret;
}

bool SchedAttr::IsSet() const { return initialized_; }

void SchedAttr::Set() const {
  if (!initialized_) {
    LOGF("Setting uninitialized scheduling attributes");
  }

  LOGD("Setting scheduling policy [" + std::to_string(sched_attr_.sched_policy) +
       "] to thread: " + std::to_string(gettid()));

  int ret = syscall(SYS_sched_setattr, 0 /* self */, &sched_attr_, 0 /* still not implemented */);
  if (ret) {
    PLOGF("Failed setting scheduling attributes \n" + to_string(sched_attr_) + "\n");
  }
}

SchedAttr& SchedAttr::operator=(const dittosuiteproto::SchedAttr& pb) {
  typedef dittosuiteproto::SchedAttr::AttributesCase SchedType;

  sched_attr_.size = sizeof(sched_attr_);

  sched_attr_.sched_flags = pb.flags();

  switch (pb.attributes_case()) {
    case SchedType::kOther:
      switch (pb.other().policy()) {
        case dittosuiteproto::SchedAttr::SchedOther::OTHER:
          sched_attr_.sched_policy = SchedPolicy::SchedNormal;
          break;
        case dittosuiteproto::SchedAttr::SchedOther::BATCH:
          sched_attr_.sched_policy = SchedPolicy::SchedBatch;
          break;
      }
      sched_attr_.sched_nice = pb.other().nice();
      break;
    case SchedType::kRt:
      switch (pb.rt().policy()) {
        case dittosuiteproto::SchedAttr::SchedRt::FIFO:
          sched_attr_.sched_policy = SchedPolicy::SchedFifo;
          break;
        case dittosuiteproto::SchedAttr::SchedRt::RR:
          sched_attr_.sched_policy = SchedPolicy::SchedRr;
          break;
      }
      if (pb.rt().priority() < 1 || pb.rt().priority() > 99) {
        LOGF("Scheduling priority should be in the range [1, 99]");
      }
      sched_attr_.sched_priority = pb.rt().priority();
      break;
    case SchedType::kDeadline:
      sched_attr_.sched_policy = SchedPolicy::SchedDeadline;
      sched_attr_.sched_runtime = pb.deadline().runtime();
      sched_attr_.sched_deadline = pb.deadline().deadline();
      sched_attr_.sched_period = pb.deadline().period();
      break;
    case SchedType::ATTRIBUTES_NOT_SET:
      LOGF("Missing scheduling attribute");
      break;
  }

  initialized_ = true;

  return *this;
}

}  // namespace dittosuite
