# Dittosuite

Dittosuite is a work in progress collection of tools that aims at providing
a high-level language called Dittolang that defines operations.

The defined Dittolang operations can be interpreted by Dittosim for simulation
to provide a simulated performance measurement and quickly identify
the goodness of a solution.

Specularly, Dittobench interprets the Dittolang operations and executes them on
a real device, tracking the behavior and measuring the performance.

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

