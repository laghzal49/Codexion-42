# Codexion

Top-level build entry for the `code_new` simulation.

## Build (from this directory)

```sh
make
```

Binary output:

```sh
./codexion
```

## Run

```sh
./codexion num_coders time_to_burnout time_to_compile time_to_debug time_to_refactor num_compiles_req dongle_cooldown scheduler
```

Example:

```sh
./codexion 5 800 200 150 150 3 100 fifo
```

## Optional: work directly inside code_new

```sh
cd code_new
make
./codexion 5 800 200 150 150 3 100 edf
```
