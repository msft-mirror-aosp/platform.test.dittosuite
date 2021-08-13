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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

const int kTimeSampleDisplayWidth = 11;  // this width is used displaying a time sample value
const int kTableWidth = 158;  // table width; can be adjusted in case of longer instruction paths
const char* kTableDivider = " | ";   // table character divider
const int kMaxHistogramHeight = 20;  // used for normalizing the histogram (represents the
                                     //  maximum height of the histogram)
const int kMaxHistogramWidth = 50;   // used for normalizing the histogram (represents the
                                     // maximum width of the histogram)
const char kCsvDelimiter = ',';      // delimiter used for .csv files
static int bin_size;                 // bin size corresponding to the normalization
                                     // of the Oy axis of the histograms

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

std::string Result::ComputeNextInstructionPath(const std::string& instruction_path) {
  return instruction_path + (instruction_path != "" ? "/" : "") + name_;
}

void Result::Print(const std::string& instruction_path) {
  std::string next_instruction_path = ComputeNextInstructionPath(instruction_path);
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
  std::cout << std::setw(18) << std::left << "Standard Deviation";
  std::cout << kTableDivider;
  PrintTableBorder();
  std::cout << "\x1b[0m";  // ending of bold
}

void PrintTimespecInTable(const timespec& t) {
  std::cout << std::setw(16) << TimespecToNs(t) << "ns";
}

// Recursive function to print one row at a time
// of statistics table content (the instruction path, min, max and mean).
void Result::PrintStatisticsTableContent(const std::string& instruction_path) {
  std::cout << "| ";  // started new row
  std::string next_instruction_path = ComputeNextInstructionPath(instruction_path);
  int subinstruction_level =
      std::count(next_instruction_path.begin(), next_instruction_path.end(), '/');
  // If the instruction path name contains too many subinstrions,
  // print only the last 2 preceded by "../".
  if (subinstruction_level > 2) {
    std::size_t first_truncate_pos = next_instruction_path.find('/');
    next_instruction_path = ".." + next_instruction_path.substr(first_truncate_pos);
  }

  std::cout << std::setw(70) << std::left << next_instruction_path << std::right;
  std::cout << kTableDivider;
  PrintTimespecInTable(min_);
  std::cout << kTableDivider;
  PrintTimespecInTable(max_);
  std::cout << kTableDivider;
  PrintTimespecInTable(mean_);
  std::cout << kTableDivider;
  PrintTimespecInTable(sd_);
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

// makes (normalized) histogram from vector
void Result::MakeHistogramFromVector(const std::vector<int>& freq_vector, const int& min_value) {
  std::cout.width(kTimeSampleDisplayWidth - 3);
  std::cout << "Time(" << time_unit_.name << ") |";
  std::cout << " Normalized number of time samples";
  std::cout << std::endl;
  for (int i = 0; i <= kMaxHistogramWidth + 15; i++) std::cout << "-";
  std::cout << std::endl;

  int sum = 0;
  int max_frequency = *std::max_element(freq_vector.begin(), freq_vector.end());
  for (unsigned int i = 0; i < freq_vector.size(); i++) {
    std::cout.width(kTimeSampleDisplayWidth);
    std::cout << min_value + bin_size * i << kTableDivider;
    for (int j = 0; j < freq_vector[i] * kMaxHistogramWidth / max_frequency; j++) std::cout << "x";
    std::cout << " {" << freq_vector[i] << "}";
    sum += freq_vector[i];
    std::cout << std::endl;
  }

  std::cout << "Total samples: { " << sum << " }" << std::endl;
  std::cout << std::endl;
}

// makes and returns the normalized frequency vector
std::vector<int> Result::ComputeNormalizedFrequencyVector() {
  int64_t min_value = TimespecToNs(min_) / time_unit_.dividing_factor;
  std::vector<int> freq_vector(kMaxHistogramHeight, 0);
  for (auto time_sample : time_samples_) {
    freq_vector[(TimespecToNs(time_sample) / time_unit_.dividing_factor - min_value) / bin_size]++;
  }
  return freq_vector;
}

Result::TimeUnit Result::GetTimeUnit(const timespec& min_value) {
  TimeUnit result;
  if (TimespecToNs(min_value) <= 1e7) {
    // time unit in nanoseconds
    result.dividing_factor = 1;
    result.name = "ns";
  } else if (TimespecToNs(min_value) <= 1e10) {
    // time unit in microseconds
    result.dividing_factor = 1e3;
    result.name = "us";
  } else if (TimespecToNs(min_value) <= 1e13) {
    // time unit in milliseconds
    result.dividing_factor = 1e6;
    result.name = "ms";
  } else {
    // time unit in seconds
    result.dividing_factor = 1e9;
    result.name = "s";
  }
  return result;
}

void Result::PrintHistograms(const std::string& instruction_path) {
  std::string next_instruction_path = ComputeNextInstructionPath(instruction_path);
  std::cout << std::endl;
  std::cout << "\x1b[1m";  // beginning of bold
  std::cout << "Instruction path: " << next_instruction_path;
  std::cout << "\x1b[0m" << std::endl;  // ending of bold
  std::cout << std::endl;

  time_unit_ = GetTimeUnit(min_);
  int64_t min_value = TimespecToNs(min_) / time_unit_.dividing_factor;
  int64_t max_value = TimespecToNs(max_) / time_unit_.dividing_factor;
  bin_size = (max_value - min_value) / kMaxHistogramHeight + 1;
  std::vector<int> freq_vector = ComputeNormalizedFrequencyVector();
  MakeHistogramFromVector(freq_vector, min_value);
  std::cout << std::endl << std::endl;

  for (const auto& sub_result : sub_results_) {
    sub_result->PrintHistograms(next_instruction_path);
  }
}

// Recursive function to print one row at a time using the .csv stream given as a parameter
// of statistics table content (the instruction path, min, max, mean and SD).
void Result::PrintStatisticInCsv(std::ostream& csv_stream, const std::string& instruction_path) {
  std::string next_instruction_path = ComputeNextInstructionPath(instruction_path);
  csv_stream << next_instruction_path << kCsvDelimiter;
  csv_stream << TimespecToNs(min_) << kCsvDelimiter;
  csv_stream << TimespecToNs(max_) << kCsvDelimiter;
  csv_stream << TimespecToNs(mean_) << kCsvDelimiter;
  csv_stream << TimespecToNs(sd_);
  csv_stream << std::endl;  // ending of row

  for (const auto& sub_result : sub_results_) {
    sub_result->PrintStatisticInCsv(csv_stream, next_instruction_path);
  }
}

void Result::MakeStatisticsCsv() {
  std::ostream csv_stream(std::cout.rdbuf());
  csv_stream << "Instruction path" << kCsvDelimiter;
  csv_stream << "Min" << kCsvDelimiter;
  csv_stream << "Max" << kCsvDelimiter;
  csv_stream << "Mean" << kCsvDelimiter;
  csv_stream << "SD" << std::endl;

  PrintStatisticInCsv(csv_stream, "");
}

}  // namespace dittosuite