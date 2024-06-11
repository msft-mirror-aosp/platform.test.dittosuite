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

#include <ditto/embedded_benchmarks.h>

#include <ditto/arg_parser.h>

namespace dittosuite {

ResultsOutput ArgToResultsOutput(const std::string_view optarg) {
  if (optarg == "report" || optarg == "0") {
    return ResultsOutput::kReport;
  }
  if (optarg == "csv" || optarg == "1") {
    return ResultsOutput::kCsv;
  }
  if (optarg == "pb" || optarg == "2") {
    return ResultsOutput::kPb;
  }
  if (optarg == "null" || optarg == "-1") {
    return ResultsOutput::kNull;
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
  if (optarg == "VERBOSE" || optarg == "5") {
    return LogLevel::kVerbose;
  }
  if (optarg == "DEBUG" || optarg == "4") {
    return LogLevel::kDebug;
  }
  if (optarg == "INFO" || optarg == "3") {
    return LogLevel::kInfo;
  }
  if (optarg == "WARNING" || optarg == "2") {
    return LogLevel::kWarning;
  }
  if (optarg == "ERROR" || optarg == "1") {
    return LogLevel::kError;
  }
  if (optarg == "FATAL" || optarg == "0") {
    return LogLevel::kFatal;
  }
  return LogLevel::kInfo;  // by default, the log level is info
}

void PrintHelpAndExit(std::string_view program_name, const std::string& help_arg) {
  if (help_arg.compare("workloads") == 0) {
    std::cerr << "The following workloads are available for execution:\n";
    for (const auto& c : ditto_static_config) {
      std::cerr << "  - " << c.first << '\n';
    }
  } else {
    std::cerr << "Usage: " << program_name << " [OPTION]... [FILE]\n"
              << "Benchmarking tool for generic workloads.\n\n"

              << "  -w, --workload[=WORKLOAD]"
              << " The workload to run. Full list with \"-h workloads\".\n"

              << "  -f, --format[=FMT]" << "\tresults output format, where FMT can be one of:\n"
              << "\t\t\t  - report (or 0, default): human readable summary;\n"
              << "\t\t\t  - csv (or 1): for comma-separated detailed stats;\n"
              << "\t\t\t  - pb (or 2): protocol-buffer text;\n"
              << "\t\t\t  - null (-1): do not print.\n"

              << "  -p, --param[=PAR]..."
              << "\tif the benchmark is parametric, all the parameters can be passed\n"
              << "\t\t\tthrough PAR (comma separated)\n"

              << "  -l, --log[=LOG]" << "\toutput stream for the log messages.\n"
              << "\t\t\tLOG can be one of: stdout (or 0, default), logcat (or 1)\n"

              << "  -v, --verbosity[=VER]" << "\toutput messages verbosity.\n"
              << "\t\t\tVER can be one of: VERBOSE (or 5), DEBUG (or 4), INFO (or 3, default),\n"
              << "\t\t\tWARNING (or 2), ERROR (or 1), FATAL (or 0)\n"

              << "  -h, --help" << "\t\tdisplay this help and exit\n\n";
  }

  exit(EXIT_SUCCESS);
}

CmdArguments ParseArguments(int argc, char** argv) {
  CmdArguments arguments;
  bool is_embedded = false;
  std::string help_arg;

  while (true) {
    int option_index = 0;
    static struct option long_options[] = {{"workload", required_argument, 0, 'w'},
                                           {"format", required_argument, 0, 'f'},
                                           {"param", required_argument, 0, 'p'},
                                           {"log", required_argument, 0, 'l'},
                                           {"verbosity", required_argument, 0, 'v'},
                                           {"help", optional_argument, 0, 'h'},
                                           {0, 0, 0, 0}};

    int c = getopt_long(argc, argv, "w:f:l:v:p:h::", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 'w':
        arguments.embedded_benchmark = optarg;
        is_embedded = true;
        break;
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
        if (!optarg && optind < argc    // make sure optind is valid
            && nullptr != argv[optind]  // make sure it's not a null string
            && '\0' != argv[optind][0]  // ... or an empty string
            && '-' != argv[optind][0]   // ... or another option
        ) {
          help_arg = argv[optind++];
        }
        [[fallthrough]];
      default: {
        PrintHelpAndExit(argv[0], help_arg);
        break;
      }
    }
  }

  if (is_embedded && optind < argc) {
    LOGE("Unexpected both embedded benchmark and .ditto file");
    PrintHelpAndExit(argv[0], help_arg);
  }
  if (!is_embedded) {
    if (optind >= argc) {
      LOGE("Expected either embedded benchmark or .ditto file");
      PrintHelpAndExit(argv[0], help_arg);
    } else {
      arguments.file_path = argv[optind];
    }
  }

  return arguments;
}

}  // namespace dittosuite
