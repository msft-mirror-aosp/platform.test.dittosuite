#!/bin/bash
#
# Copyright (C) 2024 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

unset TRACE_FILE
unset QUERY_FILE
unset WORKLOAD
unset SET_ROOT
unset PERFETTO_CONFIG_FILE
unset PERFETTO_PID
unset SERIAL


function help {
  echo "Usage: $0 [OPTION]..."
  echo "Options: "
  echo "  -h, --help          Print this message"
  echo "  -r, --root          Switch the target device to root"
  echo "  -c, --config PERFETTO_CONFIG Path to file containing the Perfetto"
  echo "                      configuration"
  echo "  -q, --query QUERY   Path to file containing the SQL query to run on"
  echo "                      the Perfetto trace with trace_processor"
  echo "  -t, --trace TRACE   Path to which the Perfetto trace will be written to"
  echo "  -w, --workload WORKLOAD Benchmark to run Dittobench with"
  echo "  -s, --serial SERIAL Serial address for ADB"
}


while [ "$#" -gt "0" ]; do
  case $1 in
    -w|--workload)
      WORKLOAD="$2"
      shift
      shift
      ;;
    -q|--query)
      QUERY_FILE="$2"
      shift
      shift
      ;;
    -t|--trace)
      TRACE_FILE="$2"
      shift
      shift
      ;;
    -c|--config)
      PERFETTO_CONFIG_FILE="$2"
      shift
      shift
      ;;
    -s|--serial)
      SERIAL="-s $2"
      shift
      shift
      ;;
    -r|--root)
      SET_ROOT=1
      shift
      ;;
    -h|--help)
      help
      exit 0
      shift
      ;;
    -*|--*)
      echo "Unknown option $1"
      help
      exit 1
      ;;
  esac
done


if [ -z "${ANDROID_HOST_OUT}" ] ; then
  echo "This script requires an Android environment. It needs to be run from an"
  echo "Android repository after the initialization scripts:"
  echo "$ source build/envsetup.sh"
  echo "$ lunch aosp_cf_x86_64_phone-trunk_staging-userdebug"
  exit 1
fi


# If the user specified a WORKLOAD, make sure that Dittobench is built, better
# before enabling tracing.
if [ ! -z "${WORKLOAD}" ] ; then
  if [ ! -z "${SET_ROOT}" ] ; then
    adb ${SERIAL} shell whoami | grep root || adb ${SERIAL} root
    sleep 2
  fi

  m dittobench
  adb ${SERIAL} push ${ANDROID_PRODUCT_OUT}/system/bin/dittobench /data/local/tmp/
fi


if [ ! -z "${PERFETTO_CONFIG_FILE}" ] ; then
  if [ -z "${TRACE_FILE}" ] ; then
    TRACE_FILE=$(mktemp)
    echo "Using temporary trace file path: \"${TRACE_FILE}\""
  fi
  ${ANDROID_BUILD_TOP}/external/perfetto/tools/record_android_trace ${SERIAL} --no-open \
    -c ${PERFETTO_CONFIG_FILE} \
    -o ${TRACE_FILE} &
  PERFETTO_PID=$!

  echo "Perfetto started (${PERFETTO_PID}), cooling down..."
  sleep 5
fi


if [ ! -z "${WORKLOAD}" ] ; then
  adb ${SERIAL} shell /data/local/tmp/dittobench -w ${WORKLOAD} -f csv | column -t -s ","
  echo "Cooldown..."
  sleep 5
fi


# If there is an instance of record_android_trace, kill it gracefully, so that
# the trace file is correctly pulled from the device.
if [ ! -z "${PERFETTO_PID}" ] ; then
  kill -s 15 $PERFETTO_PID # SIGTERM
  wait $PERFETTO_PID
fi


if [ ! -z "${QUERY_FILE}" ] ; then
  if [ -z "${TRACE_FILE}" ] ; then
    echo "A TRACE file needs to be specified to run a query, unless Perfetto is"
    echo "run as a result of setting the PERFETTO_CONFIG parameter"
    exit 1
  fi
  [ ! which trace_processor_shell ] && m trace_processor_shell
  trace_processor_shell -q ${QUERY_FILE} ${TRACE_FILE} | column -t -s ','
fi

