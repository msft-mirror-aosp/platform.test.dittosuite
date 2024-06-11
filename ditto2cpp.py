#!/bin/env python3

# Copyright (C) 2024 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse


def fetch_arguments():
  parser = argparse.ArgumentParser(
      prog="ditto2cpp",
      description="Translate .ditto file to C++ source files to embed in the benchmark."
  )
  parser.add_argument("-o", "--output", required=True, help="Output file",
                      type=str)
  parser.add_argument("-s", "--sources", required=True, nargs='+',
                      help="Source .ditto files", type=str)
  parser.add_argument("-v", "--verbose", help="Verbose output",
                      action='store_true')
  args = parser.parse_args()

  if args.verbose:
    print('From: "{}"'.format(args.sources))
    print('Output: "{}"'.format(args.output))

  return args


def compute_common_prefix(arr):
  result = arr[0]
  length = len(result)

  # Iterate for the rest of the elements in the array
  for i in range(1, len(arr)):
    # Find the index of result in the current string
    while arr[i].find(result) != 0:
      # Update the matched substring prefix
      result = result[:length - 1]
      length -= 1

      # Check for an empty case and return if true
      if not result:
        raise Exception("No results")
  return result


def generate_benchmark_source(output, sources):
  common_prefix = compute_common_prefix(sources)
  last_slash_in_prefix = 0
  try:
    last_slash_in_prefix = len(common_prefix) - common_prefix[::-1].index('/')
  except ValueError:
    # This happens when '/' cannot be found in `common_prefix`
    pass
  prefix_length = min(len(common_prefix), last_slash_in_prefix)
  suffix_length = len(".ditto")

  file_header = '''
   #include <ditto/embedded_benchmarks.h>
   const std::map<std::string, std::string> ditto_static_config = {
   '''
  file_footer = '''
   };
   '''
  with open(output, 'w') as fo:
    fo.write(file_header)
    for fi_path in sources:
      fi_name = fi_path[prefix_length:-suffix_length]
      with open(fi_path, 'r') as fi:
        fo.write('\t')
        fo.write('{{"{}", "{}"}},'.format(
            fi_name,
            fi.read().replace('\n', '').replace('"', '\\"')
        ))
        fo.write('\n')
    fo.write(file_footer)


if __name__ == '__main__':
  args = fetch_arguments()

  generate_benchmark_source(args.output, args.sources)

  if args.verbose:
    print("Output file content:")
    print('-' * 32)
    with open(args.output, 'r') as fo:
      print(fo.read())
    print('-' * 32)
