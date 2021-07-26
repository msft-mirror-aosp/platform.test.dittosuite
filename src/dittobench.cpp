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

#include <getopt.h>
#include <unistd.h>

#include <cstring>
#include <string>

#include <ditto/logger.h>
#include <ditto/parser.h>

namespace dittosuite {

static void ParseArguments(int argc, char** argv) {
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {{"results-output", required_argument, 0, 1},
                                           {"results-output-path", required_argument, 0, 2},
                                           {"log-stream", required_argument, 0, 3},
                                           {"log-delayed", no_argument, 0, 4},
                                           {"log-level", required_argument, 0, 5},
                                           {0, 0, 0, 0}};

    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 1:
        // TODO(robertasn): set results output in the output class
        break;
      case 2:
        // TODO(robertasn): set results output path in the output class
        break;
      case 3:
        // TODO(robertasn): set log stream in Logger
        break;
      case 4:
        // TODO(robertasn): set log delayed in Logger
        break;
      case 5:
        if (strcmp(optarg, "VERBOSE") == 0 || strcmp(optarg, "0") == 0) {
          dittosuite::Logger::GetInstance().SetLogLevel(dittosuite::LOG_LEVEL::LOG_LEVEL_VERBOSE);
        } else if (strcmp(optarg, "DEBUG") == 0 || strcmp(optarg, "1") == 0) {
          dittosuite::Logger::GetInstance().SetLogLevel(dittosuite::LOG_LEVEL::LOG_LEVEL_DEBUG);
        } else if (strcmp(optarg, "INFO") == 0 || strcmp(optarg, "2") == 0) {
          dittosuite::Logger::GetInstance().SetLogLevel(dittosuite::LOG_LEVEL::LOG_LEVEL_INFO);
        } else if (strcmp(optarg, "WARNING") == 0 || strcmp(optarg, "3") == 0) {
          dittosuite::Logger::GetInstance().SetLogLevel(dittosuite::LOG_LEVEL::LOG_LEVEL_WARNING);
        } else if (strcmp(optarg, "ERROR") == 0 || strcmp(optarg, "4") == 0) {
          dittosuite::Logger::GetInstance().SetLogLevel(dittosuite::LOG_LEVEL::LOG_LEVEL_ERROR);
        }
        break;
      default:
        // TODO(robertasn): print usage
        break;
    }
  }

  if (optind >= argc) {
    LOGE("Expected .ditto file");
    exit(EXIT_FAILURE);
  }

  dittosuite::Parser::GetParser().SetFilePath(argv[optind]);
}

}  // namespace dittosuite

int main(int argc, char** argv) {
  dittosuite::ParseArguments(argc, argv);

  auto instruction_set = dittosuite::Parser::GetParser().Parse();

  if (instruction_set == nullptr) {
    return 0;
  }

  instruction_set->SetUp();
  instruction_set->Run();
  instruction_set->TearDown();
  return 0;
}
