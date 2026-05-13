*This project has been created as part of the 42 curriculum by tlaghzal.    *

# Codexion

Codexion is a concurrency simulation where multiple coders compete for shared “dongles” under strict burnout deadlines. Each coder must acquire two dongles before compiling, then releases them and goes through debugging and refactoring phases. The simulation supports FIFO and EDF (earliest deadline first) scheduling to decide which coder gets priority when resources are contested.

## Description

This project models a classic resource-contention problem with deadlines. It focuses on correctness in multithreaded coordination (deadlock avoidance, fair scheduling, accurate burnout detection) while providing configurable timings for compile/debug/refactor phases and cooldowns on shared tools.

## Instructions

### Build

From the repository root:

```sh
make
```

Binary output:

```sh
./codexion
```

### Run

```sh
./codexion num_coders time_to_burnout time_to_compile time_to_debug time_to_refactor num_compiles_req dongle_cooldown scheduler
```

Arguments:
- `num_coders`: number of coder threads.
- `time_to_burnout`: deadline (ms) before a coder burns out without compiling.
- `time_to_compile`: compile duration (ms).
- `time_to_debug`: debug duration (ms).
- `time_to_refactor`: refactor duration (ms).
- `num_compiles_req`: number of compiles required per coder.
- `dongle_cooldown`: cooldown (ms) after releasing dongles.
- `scheduler`: `fifo` or `edf`.

Example:

```sh
./codexion 5 800 200 150 150 3 100 fifo
```

### Alternative (inside `code_new`)

```sh
cd code_new
make
./codexion 5 800 200 150 150 3 100 edf
```

## Blocking cases handled

- **Deadlock prevention (Coffman’s conditions)**: circular wait is prevented by enforcing a global lock order on dongles (always lock the lower-address dongle first). This removes the circular-wait condition for the two-dongle acquisition.
- **Starvation prevention**: each dongle maintains a small scheduler heap (FIFO or EDF). Only the top candidate in each heap can proceed, ensuring fair ordering and priority-aware access rather than random spinning.
- **Cooldown handling**: dongles track a `cooldown` timestamp; acquisition is denied until the cooldown expires, preventing immediate re-acquisition and encouraging fairness.
- **Precise burnout detection**: the monitor thread compares `time_to_die` against current time under per-coder locks, ensuring consistent reads of deadlines and compile counts.
- **Log serialization**: all prints are guarded by a global `log_mutex` to keep logs coherent and avoid interleaved lines.

## Thread synchronization mechanisms

- **`pthread_mutex_t` for dongles**: each dongle has a `mutex` protecting `in_use` and `cooldown`. Coders lock both dongles (with global ordering) to atomically verify availability and claim them.
- **`pthread_mutex_t` for per-dongle heaps**: each dongle has a `heap_mutex` protecting its scheduler heap (FIFO/EDF). This prevents races when multiple coders insert/remove themselves.
- **`pthread_mutex_t` for per-coder state**: each coder has `cv_mu` guarding `compile_count` and `time_to_die`, preventing races between the coder thread and the monitor.
- **`pthread_mutex_t` + `pthread_cond_t` for global coordination**: `req_mu` and `req_cv` coordinate simulation start, stop requests, and wake-ups after dongle release. This is the thread-safe communication channel between coders and the monitor/driver.
- **`pthread_mutex_t` for logging**: `log_mutex` serializes output so messages remain readable and ordered.

**Example race prevention**: when a coder updates `time_to_die` after acquiring dongles, it does so under `cv_mu`, while the monitor reads under the same lock. This guarantees the monitor never observes a partial update.

## Resources

- POSIX Threads (pthreads) overview: https://man7.org/linux/man-pages/man7/pthreads.7.html
- `pthread_mutex_t` and `pthread_cond_t`: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html and https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html
- Dining Philosophers problem (resource contention classic): https://en.wikipedia.org/wiki/Dining_philosophers_problem
- Earliest Deadline First scheduling: https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling

**AI usage**: AI assistance was used to review concurrency edge cases (lock ordering, race conditions), refine argument parsing/overflow handling, and draft/validate documentation. All changes were reviewed and adapted to match the project’s threading model and constraints.
