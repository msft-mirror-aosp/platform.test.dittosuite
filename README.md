# Dittosuite

Dittosuite is a work in progress collection of tools that aims at providing
a high-level language called Dittolang that defines operations.

The defined Dittolang operations can be interpreted by Dittosim for simulation
to provide a simulated performance measurement and quickly identify
the goodness of a solution.

Specularly, Dittobench interprets the Dittolang operations and executes them on
a real device, tracking the behavior and measuring the performance.

# How to run

```
$ ./dittobench [options] [.ditto file]
```

To run a benchmark, a well formed .ditto file must be provided, see section
[How to write .ditto files](#how-to-write-ditto-files)
In addition, these options can be set:

- `--results-output=<int | string>` (default: report). Select the results output format.
Options: report, csv with 0, 1 respectively.
- `--log-stream=<int | string>` (default: stdout). Select the output stream for the log messages.
Options: stdout, logcat with 0, 1 respectively.
- `--log-level=<int | string>` (default: INFO). Select to output messages which are at or below
the set level. Options: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL with 0, 1, 2, 3, 4 and 5
respectively.
- `--parameters=string`. If the benchmark is parametric, all the parameters (separated by commas)
can be given through this option.

# How to write .ditto files

Every .ditto file should begin with this skeleton:
```
main: {
  ...
},
global {
  ...
}
```

Optionally, it can contain `init` and `clean_up` sections:
```
init: {
  ...
},
main: {
  ...
},
clean_up: {
  ...
},
global {
  ...
}
```

## `global`

Global section should contain general benchmark configuration. Currently available options:

- (optional) `string absolute_path` (`default = ""`). Specifies the absolute path for the files.

## `init`

`init` is optional and can be used to initialize the benchmarking environment. It executes
instructions similar to `main`, but the results are not collected in the end.

## `main`

`main` is the entry point for the benchmark. It can contain a single `instruction` or
`instruction_set` (also with nested `instruction_set`).

## `clean_up`

`clean_up` is optional and can be used to reset the benchmarking environment to the initial state,
e.g, delete benchmark files. Similar to `init`, it executes instructions like `main`, but results
are not collected in the end.

## `instruction`

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
- (optional) `int repeat` (`default = 1`). Specifies how many times the instruction should be
repeated.

## `instruction_set`
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

Instruction set is an Instruction container that executes the contained instructions sequentially.
Instruction set can optionally iterate over a list and execute the provided set of instructions on
each item from the list. To use it, `iterate_options` should be set with these options:
- `string list_name` - Name of the shared variable where the list of values are stored.
- `string item_name` - Name of the shared variable to which a selected value should be stored.
- (optional) `AccessType type` (`default = SEQUENTIAL`) - Specifies if the values should be
selected sequentially or randomly. Options: `SEQUENTIAL`, `RANDOM`.
- (optional) `Reseeding reseeding` (`default = ONCE`) - Specifies how often the random number
generator should be reseeded with the same provided (or generated) seed. Options: `ONCE`,
`EACH_ROUND_OF_CYCLES`, `EACH_CYCLE`.
- (optional) `uint32 seed` - Seed for the random number generator. If the seed is not provided,
current system time is used as the seed.

## `multithreading` and `threads`

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

Multithreading is another instruction container that executes the specified instructions
(or instruction sets) in different threads. If the optional `spawn` option for a specific
instruction (or instruction set) is provided, then the provided number of threads will be created
for it.

## Example

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


# Use cases


## File operations performance (high priority)

Bandwidth and measurement when dealing with few huge files or many small files.
Operations are combinations of sequential/random-offset read/write.

Latency in creating/deleting files/folders.

These operations should be able to be triggered in a multiprogrammed fashion.


## Display pipeline

A graph of processes that are communicating with each others in a pipeline of
operations that are parallely contributing to the generation of display frames.


## Scheduling (low priority)

Spawning tasks (period, duration, deadline) and verifying their scheduling
latency and deadline misses count.


# Workflow example and implementation nits

In the following scenario, two threads are running.

T1 runs the following operations: Read, Write, Read, sends a request to T2 and
waits for the reply, then Write, Read.
T2 waits for a request, then Read, Write, then sends the reply to the requester.

Operations are encoded as primitives expressed with ProtoBuffers.
The definition of dependencies among threads can be represented as graphs.


## Initialization phase

The first graph traversal is performed at initialization time, when all the
ProtoBuffer configuration files are distributed among all the binaries so that
they can perform all the heavy initialization duties.


## Execution phase

After the initialization phase completes the graph can be traversed again to
put all the workloads in execution.


## Results gathering phase

A final graph traversal can be performed to fetch all the measurements that
each entity internally stored.


## Post processing

All the measurements must be ordered and later processed to provide useful
information to the user.

T1: INIT   : [ RD WR RD ]  SND               RCV [ WR RD ] : END
T2:   INIT :                 RCV [ RD WR ] SND             :   END


# Scratch notes

critical path [ READ WRITE READ ] [ READ WRITE ]  [ WRITE READ ]
-------------------->
             >                  <
Thread1   III-XXXXXX|X-SSSSSS-XX-TTTT
Thread2               III-XXXX|XXX-TTTT
                     ^

       >       XXXXXXX    XX<
                      XXXX


READ WRITE READ

--->

vector<instr*> {read(), write(), read()};
-> start()


RECEIVE READ WRITE READ SEND
--->
vector<instr*> {receive(), read(), write(), read(), send()};
start()

lock on receive()

