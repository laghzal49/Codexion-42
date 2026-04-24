# Codexion

Threaded coding simulation using pthreads.

## Build

```sh
make
```

This generates the executable `codexion`.

## Run

```sh
./codexion num_coders time_to_burnout time_to_compile time_to_debug time_to_refactor num_compiles_req dongle_cooldown scheduler
```

Example:

```sh
./codexion 5 800 200 150 150 3 100 fifo
```

## Parameters

- `num_coders`: number of coder threads.
- `time_to_burnout`: max time (ms) without a compile before burnout.
- `time_to_compile`: compile duration (ms).
- `time_to_debug`: debug duration (ms).
- `time_to_refactor`: refactor duration (ms).
- `num_compiles_req`: required compiles per coder.
- `dongle_cooldown`: cooldown (ms) after releasing a dongle.
- `scheduler`: `fifo` or `edf` (accepted by parser).

## Useful Targets

- `make`: build.
- `make clean`: remove object files.
- `make fclean`: remove objects and binary.
- `make re`: full rebuild.

## Notes

- Very tight timing combinations can still cause burnout by design.
- Example of a stable stress test:

```sh
./codexion 200 5000 10 10 5 1 60 edf
```
