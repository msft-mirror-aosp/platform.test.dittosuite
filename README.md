# Dittosuite

Dittosuite is a collection of tools that simplifies writing and running complex
workloads.

These workloads are defined by a set of instructions including, but not limited
to, CPU load, memory allocations, file system operations, Binder IPC, that can
be combined to run sequentially or in parallel.

A new high-level language, Dittolang, defines the workload in the form of a
textual representation of Protocol Buffers (aka .ditto files).

The operations defined in the .ditto files are interpreted and executed by
Dittobench on the target device.

## Benchmarking

During the Dittobench execution of the instructions defined in the .ditto file,
the tool tracks performance metrics such as execution time and I/O bandwidth
(more in the [Sampling](#sampling) section).

# Doxygen documentation

In addition to this readme, the project includes a Doxygen configuration, which
can be generated with the following command:

```
$ doxygen
```

# How to run

```
$ ./dittobench [options] [.ditto file]
```

To run a benchmark, a well formed .ditto file must be provided, see section
[How to write .ditto files](#writing-ditto-files)
In addition, these options can be set:

- `--results-output=<string>` (default: report). Select the results output
  format. Options: report, csv.
- `--log-stream=<string>` (default: stdout). Select the output stream for
  the log messages. Options: stdout, logcat.
- `--log-level=<string>` (default: INFO). Select to output messages which are
  at or below the set level. Options: VERBOSE, DEBUG, INFO , WARNING, ERROR,
  FATAL.
- `--parameters=string`. If the benchmark takes parameters, they can be passed
  through this option, separated by commas.

# Writing .ditto files {#writing-ditto-files}

## Sections

Every .ditto file should begin with this skeleton:

```
main: {},
global {}
```

Optionally, it can contain `init` and `clean_up` sections:

```
main: {},
init: {},
clean_up: {},
global {}
```

### `global`

The `global` section contains general benchmark parameters.
Available options:

- (optional) `string absolute_path` (`default = ""`). Specifies the absolute
  path for files created by benchmarks. This parameter simplifies the
  definition of paths among different file system `Instruction`s, for example,
  when different benchmarks should be run in different paths with different
  file systems.

### `main`

`main` is the entry point for the benchmark.

It contains a single `instruction`.

Having a single instruction does not mean that the tool is limited to one
instruction. In fact, as will be explained later, multiple instances of the
same instruction can be executed with the parameter `repeat`, or a series of
different instructions can be execute using the special instruction
`instruction_set`.

### `init` (optional)

`init` initializes the benchmarking environment.

It executes instructions similar to `main`, but the results are not collected
at the end of the execution.

### `clean_up` (optional)

`clean_up` is optional and can be used to reset the benchmarking environment to
the initial state, e.g, delete benchmark files. Similar to `init`, it executes
instructions like `main`, but results are not collected in the end.

## Instructions

Every workload is composed of one or more instructions.

Almost everything in Ditto `main` itself is an instruction!

Here is an example of a .ditto file.

```
main: {
  instruction_set: {
    instructions: [
      {
        open_file: {
          path_name: "newfile2.txt",
          output_fd: "test_file"
        }
      },
      {
        close_file: {
          input_fd: "test_file"
        }
      }
    ]
  },
  repeat: 10
},
global {
  absolute_path: "/data/local/tmp/";
}
```

See more examples in `example/`.

### `instruction`

```
{
  <name of the instruction>: {
    <first argument>,
    <second argument>,
    ...
  },
  <general instruction options>
}
```

Currently available options:

- (optional) `int repeat` (`default = 1`). Specifies how many times the
  instruction should be repeated.

### `instruction_set`

```
{
  instruction_set: {
    instructions: {
      {
        <name of the first instruction>: {
          <first argument>,
          <second argument>,
          ...
        },
        <general instruction options>
      },
      {
        <name of the second instruction>: {
          <first argument>,
          <second argument>,
          ...
        },
        <general instruction options>
      },
      ...
    },
    iterate_options: {...}
  },
  <general instruction options>
}
```

Instruction set is an Instruction container that executes the contained
instructions sequentially.

Instruction set can optionally iterate over a list and execute the provided set
of instructions on each item from the list. To use it, `iterate_options` should
be set with these options:

- `string list_name` - Shared variable name pointing to a list of values.
- `string item_name` - Shared variable name to which a selected value should be
  stored.
- (optional) `Order order` (`default = SEQUENTIAL`) - Specifies if
  the elements of the list should be accessed sequentially or randomly. Options:
  `SEQUENTIAL`, `RANDOM`.
- (optional) `Reseeding reseeding` (`default = ONCE`) - Specifies how often the
  random number generator should be reseeded with the same provided (or
  generated) seed.  Options: `ONCE`, `EACH_ROUND_OF_CYCLES`, `EACH_CYCLE`.
- (optional) `uint32 seed` - Seed for the random number generator. If the seed
  is not provided,
  current system time is used as the seed.

### `multithreading`

```
multithreading: {
  threads: [
    {
      instruction: {...},
      spawn: <number of threads to spawn with the provided instruction>
    },
    ...
  ]
}
```

Multithreading is another instruction container that executes the specified
instructions (or instruction sets) in different threads. If the optional
`spawn` option for a specific instruction (or instruction set) is provided,
then the provided number of threads will be created for it.

Arguments:

- `Thread threads`: an array of `Thread`s that will be executed in parallel.
  Each `Thread` specifies:
  - the `Instruction` to run.
  - (optional, default 1) `int32 spawn`: number of threads/processes to be
    created for this instruction.
  - (optional) `string name`: alias name to be assigned to the thread/process.
  - (optional) `SchedAttr sched_attr`: scheduling parameters.
  - (optional, default -1) `int64 sched_affinity`: bitmask that defines what
    CPUs the thread/process can run on.
- (optional, default false) `bool fork`: if true, creates processes, otherwise
  creates threads.

### `open_file`

Opens the file specified by the given path or by a shared variable name
pointing to a file path. If neither of those are provided, a random 9-digit
name is generated.  Optionally saves the file descriptor which can then be used
by subsequent instructions. Also, can optionally create the file if it does not
already exist.

Arguments:

- (optional) `string path_name` - Specifies the file path.
- (OR, optional) `string input` - Shared variable name pointing to a file path.
- (optional) `string output_fd` - Shared variable name to which output file
  descriptor should be saved.
- (optional) `bool create` (`default = true`) - Specifies if the file should be
  created if it does not already exist. If the file exists, nothing happens.

### `delete_file`

Deletes the file with a file path or a shared variable name pointing to a file
path.  Uses `unlink(2)`.

Arguments:

- `string path_name` - Specifies the file path.<br/>
  OR<br/>
  `string input` - Shared variable name pointing to a file path.

### `close_file`

Closes the file with the provided file descriptor.
Uses `close(2)`.

Arguments:

- `string input_fd` - Shared variable name pointing to a file descriptor.

### `resize_file`

Resizes the file with the provided file descriptor and new size.  If the
provided size is greater than the current file size, `fallocate(2)` is used,
otherwise `ftruncate(2)` is used.

Arguments:

- `string input_fd` - Shared variable name pointing to a file descriptor.
- `int64 size` - New file size (in bytes).

### `resize_file_random`

Resizes the file with the provided file descriptor and a range for the new
size. The new file size is randomly generated in the provided range and if the
generated size is greater than the current file size, `fallocate(2)` is used,
otherwise `ftruncate(2)` is used.

Arguments:

- `string input_fd` - Shared variable name pointing to a file descriptor.
- `int64 min` - Minimum value (in bytes)
- `int64 max` - Maximum value (in bytes)
- (optional) `uint32 seed` - Seed for the random number generator. If the seed
  is not provided, current system time is used as the seed.
- (optional) `Reseeding reseeding` (`default = ONCE`). How often the random
  number generator should be reseeded with the provided (or generated) seed.
  Options: `ONCE`, `EACH_ROUND_OF_CYCLES`, `EACH_CYCLE`.

### `write_file`

Writes to file with the provided file descriptor. For `SEQUENTIAL` access, the
blocks of data will be written sequentially and if the end of the file is
reached, new blocks will start from the beginning of the file. For `RANDOM`
access, the block offset, to which data should be written, will be randomly
chosen with uniform distribution. `10101010` byte is used for the write
operation to fill the memory with alternating ones and zeroes. Uses
`pwrite64(2)`.

Arguments:

- `string input_fd` - Shared variable name pointing to a file descriptor.
- (optional) `int64 size` (`default = -1`) - How much data (in bytes) should be
  written in total.  If it is set to `-1`, then file size is used.
- (optional) `int64 block_size` (`default = 4096`) - How much data (in bytes)
  should be written at once. If it is set to `-1`, then file size is used.
- (optional) `int64 starting_offset` (`default = 0`) - If `access_order` is set
  to `SEQUENTIAL`, then the blocks, to which the data should be written, will
  start from this starting offset (in bytes).
- (optional) `Order access_order` (`default = SEQUENTIAL`) - Order of the
  write. Options: `SEQUENTIAL` and `RANDOM`.
- (optional) `uint32 seed` - Seed for the random number generator. If the seed
  is not provided, current system time is used as the seed.
- (optional) `bool fsync` (`default = false`) - If set, `fsync(2)` will be
  called after the execution of all write operations.
- (optional) `Reseeding reseeding` (`default = ONCE`) - How often the random
  number generator should be reseeded with the provided (or generated) seed.
  Options: `ONCE`, `EACH_ROUND_OF_CYCLES`, `EACH_CYCLE`.

### `read_file`

Reads from file with the provided file descriptor. For `SEQUENTIAL`
access, the blocks of data will be read sequentially and if the end of
the file is reached, new blocks will start from the beginning of the file. For
`RANDOM` access, the block offset, from which data should be read, will
be randomly chosen with uniform distribution. Calls `posix_fadvise(2)` before
the read operations. Uses `pread64(2)`.

Arguments:

- `string input_fd` - Shared variable name pointing to a file descriptor.
- (optional) `int64 size` (`default = -1`) - How much data (in bytes) should be
  read in total.  If it is set to `-1`, then file size is used.
- (optional) `int64 block_size` (`default = 4096`) - How much data (in bytes)
  should be read at once. If it is set to `-1`, then file size is used.
- (optional) `int64 starting_offset` (`default = 0`) - If `access_order` is set
  to `SEQUENTIAL`, then the blocks, from which the data should be read, will
  start from this starting offset (in bytes).
- (optional) `Order access_order` (`default = SEQUENTIAL`) - Order of the read.
  Options: `SEQUENTIAL` and `RANDOM`.
- (optional) `uint32 seed` - Seed for the random number generator. If the seed
  is not provided, current system time is used as the seed.
- (optional) `ReadFAdvise fadvise` (`default = AUTOMATIC`) - Sets the argument
  for the `posix_fadvise(2)` operation. Options: `AUTOMATIC`, `NORMAL`,
  `SEQUENTIAL` and `RANDOM`. If `AUTOMATIC` is set, then
  `POSIX_FADV_SEQUENTIAL` or `POSIX_FADV_RANDOM` will be used for `SEQUENTIAL`
  and `RANDOM` access order respectively.
- (optional) `Reseeding reseeding` (`default = ONCE`) - How often the random
  number generator should be reseeded with the provided (or generated) seed.
  Options: `ONCE`, `EACH_ROUND_OF_CYCLES`, `EACH_CYCLE`.

### `read_directory`

Reads file names from a directory and stores them as a list in a shared
variable. Uses `readdir(3)`.

Arguments:

- `string directory_name` - Name of the directory
- `string output` - Shared variable name to which files names should be saved.

### `invalidate_cache`

Drops kernel caches, including dentry, inode and page caches. This is done by
calling `sync()` and then writing `3` to `/proc/sys/vm/drop_caches`.

# Sampling {#sampling}

TODO

# Dependencies

## Android

The project is currently being developed as part of the Android Open Source
Project (AOSP) and is supposed to run out-of-the-box.

## Linux

The following utilities are required to build the project on Linux:

```
sudo apt install cmake protobuf-compiler

```

# Testing

## Linux

A suite of unit tests is provided in the test/ directory.
In Linux these tests can be run with the following commands:

```
mkdir build
cd build
make
cd test
ctest
```

### Coverage

As an extension to testing, coverage is not a metric that guarantees good
quality testing, but at least shows what is not been tested yet.

One way of getting coverage data is to rely on `llvm` to build the code with
extra flags to generate coverage information, `llvm-cov` to extract coverage
data, and `lcov` to aggregate and export all the coverage information into a
human-readable format.

```
mkdir build
cd build
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake -DCMAKE_BUILD_TYPE=Debug ..
make
ctest --test-dir test
lcov -d ./CMakeFiles/ -b . --gcov-tool $PWD/../test/llvm-gcov.sh --capture -o cov.info
genhtml cov.info -o coverage_html
```

> **_NOTE:_**: `lcov` version `2.0-1` has issues such as `geninfo: ERROR: "XXX:
> function YYY found on line but no corresponding 'line' coverage data point.
> Cannot derive function end line.` This can be solved by downgrading to
> version 1.6. The lcov repository already has a binary, so `PATH` can be
> updated with its `bin` folder.

