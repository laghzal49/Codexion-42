## Codexion-42

A compact C project implementing a custom heap allocator and related tooling,
built as a 42-school style project. This repository demonstrates low-level
C programming, memory management, and tooling useful for a systems programming
portfolio.

## Key Highlights

- Implemented a custom heap allocator (`ft_malloc.c`, `heap.c`, `heap_utils.c`).
- Helpers and monitoring tooling for allocator behavior (`monitor.c`, `dongle.c`).
- Clear modular structure separating parsing, state management and runtime.

## Files of Interest

- `ft_malloc.c`, `heap.c`, `heap_utils.c` — allocator implementation.
- `monitor.c`, `dongle.c`, `dongle_helper.c` — runtime monitoring and helpers.
- `parsing.c`, `init.c`, `starter.c` — program initialization and CLI parsing.

## Build & Run

Build with the included Makefile:

```bash
make
```

Then run the built binary (example):

```bash
./codexion
```

## Project Overview

`Codexion-42` contains a concurrency simulation combined with a custom memory
allocator and monitoring utilities. It was developed as an exercise in
multithreading, synchronization, and low-level memory management, with a focus
on robust behavior under contention and clear logging for analysis.

## Features

- Custom heap allocator and utilities (`ft_malloc.c`, `heap.c`, `heap_utils.c`).
- Concurrency-safe resource acquisition simulation (coders and dongles).
- Scheduling policies (FIFO and EDF) for access arbitration under contention.
- Runtime monitor and logging to analyze timing and state transitions.

## Detailed Usage

Run the simulator with the following CLI arguments:

```
./codexion num_coders time_to_burnout time_to_compile time_to_debug time_to_refactor num_compiles_req dongle_cooldown scheduler
```

Where:
- `num_coders` — number of coder threads.
- `time_to_burnout` — deadline (ms) before a coder burns out without compiling.
- `time_to_compile` — compile duration (ms).
- `time_to_debug` — debug duration (ms).
- `time_to_refactor` — refactor duration (ms).
- `num_compiles_req` — number of compiles required per coder.
- `dongle_cooldown` — cooldown (ms) after releasing dongles.
- `scheduler` — `fifo` or `edf`.

Example:

```bash
./codexion 5 800 200 150 150 3 100 fifo
```

## Architecture & Design Notes

- Dongles are modeled as shared resources with per-dongle mutexes and
	scheduling heaps to avoid starvation and support priority-based selection.
- Deadlock prevention is implemented via a strict lock ordering when acquiring
	multiple dongles.
- The allocator code is separated from simulation logic to make it easier to
	test and reuse in other contexts.

## Development

To build (development):

```bash
make
```

Run tests or small scenarios by invoking the binary with different parameters
or by adding small test drivers in `coders/` to exercise the allocator.

## Contributing

Contributions are welcome. Open an issue or submit a pull request with a clear
description of the change, rationale, and any test cases or example runs.

## License & Attribution

This repository was created as part of the 42 curriculum. Use and modification
is permitted for educational and portfolio purposes. If you reuse code in a
public portfolio, add a short note attributing the origin and your modifications.
