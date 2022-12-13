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

#include <ditto/shared_variables.h>

#include <ditto/logger.h>

namespace dittosuite {

// Matches variable_name to the integer key value.
//
// If variable_name already exists in the map, then the key is returned.
//
// If variable_name does not exist in the map, size of the vector of shared_variables
// is increased by one and the new key (index of the last element in the resized vector)
// is saved together with variable_name in the map and returned.
int SharedVariables::GetKey(const std::string& variable_name) {
  if (keys_.find(variable_name) == keys_.end()) {
    int key = variables_.size();
    keys_.insert({variable_name, key});
    variables_.resize(variables_.size() + 1);
    return key;
  }
  return SharedVariables::keys_[variable_name];
}

SharedVariables::Variant SharedVariables::Get(int key) {
  if (key < 0 || static_cast<unsigned int>(key) >= variables_.size()) {
    LOGE("Shared variable with the provided key does not exist");
    exit(EXIT_FAILURE);
  }
  return variables_[key];
}

SharedVariables::Variant SharedVariables::Get(const std::string& variable_name) {
  return Get(GetKey(variable_name));
}

void SharedVariables::Set(int key, const SharedVariables::Variant& value) {
  if (key < 0 || static_cast<unsigned int>(key) >= variables_.size()) {
    LOGE("Shared variable with the provided key does not exist");
    exit(EXIT_FAILURE);
  }
  variables_[key] = value;
}

void SharedVariables::Set(const std::string& variable_name, const Variant& value) {
  Set(GetKey(variable_name), value);
}

void SharedVariables::ClearKeys() {
  keys_.clear();
}

std::vector<SharedVariables::Variant> SharedVariables::variables_;
std::unordered_map<std::string, int> SharedVariables::keys_;

}  // namespace dittosuite
