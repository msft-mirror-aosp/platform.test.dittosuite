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

ResultsOutput ArgToResultsOutput(const std::string_view optarg) {
  if (optarg == "csv" || optarg == "1") {
    return ResultsOutput::kCsv;
  }
  return ResultsOutput::kReport;  // by default, the results output is the report (= 0)
}

LogStream ArgToLogStream(const std::string_view optarg) {
  if (optarg == "logcat" || optarg == "1") {
#ifdef __ANDROID__
    return LogStream::kLogcat;
#else
    PLOGF("Cannot set log stream as logcat outside of Android");
#endif
  }
  return LogStream::kStdout;  // by default, the log stream is stdout
}

LogLevel ArgToLogLevel(const std::string_view optarg) {
  if (optarg == "VERBOSE" || optarg == "0") {
    return LogLevel::kVerbose;
  }
  if (optarg == "DEBUG" || optarg == "1") {
    return LogLevel::kDebug;
  }
  if (optarg == "INFO" || optarg == "2") {
    return LogLevel::kInfo;
  }
  if (optarg == "WARNING" || optarg == "3") {
    return LogLevel::kWarning;
  }
  if (optarg == "ERROR" || optarg == "4") {
    return LogLevel::kError;
  }
  if (optarg == "FATAL" || optarg == "5") {
    return LogLevel::kFatal;
  }
  return LogLevel::kInfo;  // by default, the log level is info
}

void PrintHelpAndExit(char **argv) {
  std::cerr << "Usage: " << argv[0] << " [OPTION]... [FILE]" << std::endl;
  std::cerr << "Benchmarking tool for generic workloads." << std::endl;
  std::cerr << std::endl;

  std::cerr << "  -f, --format[=FMT]";
  std::cerr << "\tresults output format." << std::endl;
  std::cerr << "\t\t\tFMT can be: report (or 0, default), csv (or 1)" << std::endl;

  std::cerr << "  -p, --param[=PAR]...";
  std::cerr << "\tif the benchmark is parametric, all the parameters can be passed" << std::endl;
  std::cerr << "\t\t\tthrough PAR (comma separated)" << std::endl;

  std::cerr << "  -l, --log[=LOG]";
  std::cerr << "\toutput stream for the log messages." << std::endl;
  std::cerr << "\t\t\tLOG can be one of: stdout (or 0, default), logcat (or 1)" << std::endl;

  std::cerr << "  -v, --verbosity[=VER]";
  std::cerr << "\toutput messages verbosity." << std::endl;
  std::cerr << "\t\t\tVER can be one of: VERBOSE (or 5), DEBUG (or 4), INFO (or 3, default)," << std::endl;
  std::cerr << "\t\t\tWARNING (or 2), ERROR (or 1), FATAL (or 0)" << std::endl;

  std::cerr << "  -h, --help";
  std::cerr << "\t\tdisplay this help and exit" << std::endl;

  exit(EXIT_SUCCESS);
}

CmdArguments ParseArguments(int argc, char** argv) {
  CmdArguments arguments;

  while (true) {
    int option_index = 0;
    static struct option long_options[] = {{"format", required_argument, 0, 'f'},
                                           {"param", required_argument, 0, 'p'},
                                           {"log", required_argument, 0, 'l'},
                                           {"verbosity", required_argument, 0, 'v'},
                                           {"help", no_argument, 0, 'h'},
                                           {0, 0, 0, 0}};

    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 'f':
        arguments.results_output = ArgToResultsOutput(optarg);
        break;
      case 'l':
        dittosuite::Logger::GetInstance().SetLogStream(ArgToLogStream(optarg));
        break;
      case 'v':
        dittosuite::Logger::GetInstance().SetLogLevel(ArgToLogLevel(optarg));
        break;
      case 'p': {
        char* token = strtok(optarg, ",");
        while (token != nullptr) {
          arguments.parameters.push_back(token);
          token = strtok(nullptr, ",");
        }
        break;
      }
      case 'h':
        [[fallthrough]];
      default: {
        PrintHelpAndExit(argv);
        break;
      }
    }
  }

  if (optind >= argc) {
    LOGE("Expected .ditto file");
    PrintHelpAndExit(argv);
  }

  arguments.file_path = argv[optind];

  return arguments;
}

}  // namespace dittosuite
