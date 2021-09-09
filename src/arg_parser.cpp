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

#include <ditto/arg_parser.h>

namespace dittosuite {

ResultsOutput ArgToResultsOutput(char* optarg) {
  if (strcmp(optarg, "csv") == 0 || strcmp(optarg, "1") == 0) return kCsv;
  return kReport;  // by default, the results output is the report (= 0)
}

LOG_STREAM ArgToLogStream(char* optarg) {
  if (strcmp(optarg, "logcat") == 0 || strcmp(optarg, "1") == 0) {
#ifdef __ANDROID__
    return LOG_STREAM_LOGCAT;
#endif
    PLOGF("Cannot set log stream as logcat outside of Android");
  }
  return LOG_STREAM_STDOUT;  // by default, the log stream is stdout
}

LOG_LEVEL ArgToLogLevel(char* optarg) {
  if (strcmp(optarg, "VERBOSE") == 0 || strcmp(optarg, "0") == 0)
    return LOG_LEVEL_VERBOSE;
  else if (strcmp(optarg, "DEBUG") == 0 || strcmp(optarg, "1") == 0)
    return LOG_LEVEL_DEBUG;
  else if (strcmp(optarg, "INFO") == 0 || strcmp(optarg, "2") == 0)
    return LOG_LEVEL_INFO;
  else if (strcmp(optarg, "WARNING") == 0 || strcmp(optarg, "3") == 0)
    return LOG_LEVEL_WARNING;
  else if (strcmp(optarg, "ERROR") == 0 || strcmp(optarg, "4") == 0)
    return LOG_LEVEL_ERROR;
  else if (strcmp(optarg, "FATAL") == 0 || strcmp(optarg, "5") == 0)
    return LOG_LEVEL_FATAL;
  return LOG_LEVEL_INFO;  // by default, the log level is info
}

CmdArguments ParseArguments(int argc, char** argv) {
  CmdArguments arguments;
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {{"results-output", required_argument, 0, 1},
                                           {"log-stream", required_argument, 0, 2},
                                           {"log-delayed", no_argument, 0, 3},
                                           {"log-level", required_argument, 0, 4},
                                           {0, 0, 0, 0}};

    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 1:
        arguments.results_output = ArgToResultsOutput(optarg);
        break;
      case 2:
        dittosuite::Logger::GetInstance().SetLogStream(ArgToLogStream(optarg));
        break;
      case 3:
        // TODO(robertasn): set log delayed in Logger
        break;
      case 4:
        dittosuite::Logger::GetInstance().SetLogLevel(ArgToLogLevel(optarg));
        break;
      default:
        // TODO(robertasn): print usage
        break;
    }
  }

  if (optind >= argc) {
    LOGF("Expected .ditto file");
  }

  arguments.file_path = argv[optind];

  return arguments;
}

}  // namespace dittosuite
