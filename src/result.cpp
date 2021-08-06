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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

const int kTableWidth = 137;  // table width; can be adjusted in case of longer instruction paths
const char* kTableDivider = " | ";  // table character divider

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
  sd_ = StatisticsGetSd(time_samples_);
}

void Result::Print(const std::string& instruction_path) {
  std::string next_instruction_path =
      instruction_path + (instruction_path != "" ? "/" : "") + name_;
  std::cout << next_instruction_path << std::endl;
  std::cout << "Min: " << min_.tv_sec << "s, " << min_.tv_nsec << "ns" << std::endl;
  std::cout << "Max: " << max_.tv_sec << "s, " << max_.tv_nsec << "ns" << std::endl;
  std::cout << "Mean: " << mean_.tv_sec << "s, " << mean_.tv_nsec << "ns" << std::endl;
  std::cout << "SD: " << sd_.tv_sec << "s, " << sd_.tv_nsec << "ns" << std::endl << std::endl;

  for (const auto& sub_result : sub_results_) {
    sub_result->Print(next_instruction_path);
  }
}

void PrintTableBorder() {
  std::cout << std::endl;
  for (int i = 0; i < kTableWidth; i++) std::cout << "-";
  std::cout << std::endl;
}

void PrintStatisticsTableHeader() {
  std::cout << "\x1b[1m";  // beginning of bold
  PrintTableBorder();
  std::cout << "| ";  // beginning of table row
  std::cout << std::setw(70) << std::left << "Instruction name";
  std::cout << kTableDivider;
  std::cout << std::setw(18) << std::left << " Min";
  std::cout << kTableDivider;
  std::cout << std::setw(18) << std::left << " Max";
  std::cout << kTableDivider;
  std::cout << std::setw(18) << std::left << " Mean";
  std::cout << kTableDivider;
  PrintTableBorder();
  std::cout << "\x1b[0m";  // ending of bold
}

void PrintTimespecInTable(const timespec& t) {
  std::cout << std::setw(3) << t.tv_sec << "s, ";
  std::cout << std::setw(10) << t.tv_nsec << "ns";
}

// Recursive function to print one row at a time
// of statistics table content (the instruction path, min, max and mean).
void Result::PrintStatisticsTableContent(const std::string& instruction_path) {
  std::cout << "| ";  // started new row
  std::string next_instruction_path =
      instruction_path + (instruction_path != "" ? "/" : "") + name_;
  int subinstruction_level =
      std::count(next_instruction_path.begin(), next_instruction_path.end(), '/');
  // If the instruction path name contains too many subinstrions,
  // print only the last 2 preceded by "../".
  if (subinstruction_level > 2) {
    std::size_t first_truncate_pos = next_instruction_path.find("/");
    next_instruction_path = ".." + next_instruction_path.substr(first_truncate_pos);
  }

  std::cout << std::setw(70) << std::left << next_instruction_path << std::right;
  std::cout << kTableDivider;
  PrintTimespecInTable(min_);
  std::cout << kTableDivider;
  PrintTimespecInTable(max_);
  std::cout << kTableDivider;
  PrintTimespecInTable(mean_);
  std::cout << kTableDivider;  // ended current row
  PrintTableBorder();
  for (const auto& sub_result : sub_results_) {
    sub_result->PrintStatisticsTableContent(next_instruction_path);
  }
}

void Result::PrintStatisticsTable() {
  PrintStatisticsTableHeader();
  PrintStatisticsTableContent("");
}
}  // namespace dittosuite