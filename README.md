# Codexion: Master the Race for Resources

*This project has been created as part of the 42 curriculum by tlaghzal.*

## Description

**Codexion** is an advanced multi-threaded simulation of the Dining Philosophers problem, implemented in C with a focus on concurrent programming, resource management, and senior-level synchronization patterns.

### Goal

The project simulates a scenario where coders compete for limited dongles (shared resources/forks) to compile code. Each coder must manage:
- **Compiling**: Requires both left and right dongle (fork)
- **Debugging**: A solo activity requiring no dongles
- **Refactoring**: Another solo activity requiring no dongles
- **Burnout prevention**: Coders must compile within a deadline to avoid burnout

The challenge is to implement a deadlock-free, starvation-free scheduler that manages resource allocation efficiently using two different scheduling policies:
- **FIFO**: First-In-First-Out (simple fairness)
- **EDF**: Earliest Deadline First (priority-based scheduling)

### Key Features

- **Phase 1**: Priority Queue (FIFO/EDF) based resource scheduling
- **Phase 2**: Interrupt-driven timing engine using `pthread_cond_timedwait` (0% CPU usage during sleep)
- **Thread-safe logging**: Serialized output with mutex protection
- **Precise burnout detection**: Monitor thread with synchronized state checking
- **Deadlock prevention**: Ordered resource acquisition (left-first for odd coders, right-first for even)
- **42 School Norm Compliance**: Max 25 lines per function, no global variables, clean architecture

---

## Instructions

### Prerequisites

- GCC compiler with pthread support
- Linux/Unix environment
- Make utility

### Compilation

```bash
cd coders
make
```

This will compile all source files and produce the `codexion` binary.

### Usage

```bash
./codexion <num_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <compiles_required> <dongle_cooldown> <scheduler>
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `num_coders` | int | Number of coders (philosophers) in the simulation |
| `time_to_burnout` | ms | Maximum time before a coder burns out (deadline) |
| `time_to_compile` | ms | Time each coder spends compiling |
| `time_to_debug` | ms | Time each coder spends debugging |
| `time_to_refactor` | ms | Time each coder spends refactoring |
| `compiles_required` | int | Number of successful compiles required per coder to win |
| `dongle_cooldown` | ms | Cooldown period after releasing a dongle |
| `scheduler` | string | Scheduling algorithm: `fifo` or `edf` |

#### Example Usage

```bash
# 5 coders, 5-second burnout, 100ms compile, 100ms debug, 50ms refactor, 3 compiles needed, 100ms cooldown, FIFO scheduler
./codexion 5 5000 100 100 50 3 100 fifo

# 4 coders, 2-second burnout, EDF scheduler
./codexion 4 2000 80 80 40 2 50 edf
```

### Output

The program prints each state transition in the format:
```
<timestamp_ms> <coder_id> <state>
```

Example output:
```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
100 1 is debugging
200 1 is refactoring
400 1 has taken a dongle
...
```

### Cleanup

```bash
make clean   # Remove object files
make fclean  # Remove object files and binary
make re      # Rebuild from scratch
```

---

## Thread Synchronization Mechanisms

### 1. **Dongle Access Synchronization**

#### Problem
Multiple coders compete for limited dongles. Without proper synchronization, race conditions would occur:
- Dongle could be taken by multiple coders simultaneously
- State inconsistencies when checking availability

#### Solution: `dongle.c` - Mutex + Condition Variable + Priority Queue

```c
struct s_dongle {
    pthread_mutex_t  mutex;           /* Protects all dongle state */
    pthread_cond_t   cond;            /* Signals availability */
    int              available_flag;  /* Is dongle free? */
    long long        cooldown_timestamp; /* When can it be reused? */
    t_pq             queue;            /* Queue of waiting coders */
};
```

**Implementation** (`take_dongles` function):

```
1. Lock dongle->mutex
2. Add coder to priority queue (using static push_request_to_queue)
3. Wait via pthread_cond_wait until:
   - Dongle is available (available_flag == 1)
   - AND coder's request is at queue front (fairness)
4. Wait for cooldown period to expire
5. Mark dongle as taken (available_flag = 0)
6. Remove coder from queue
7. Unlock mutex
```

**Race Condition Prevention**:
- The mutex ensures only one coder can modify dongle state at a time
- The condition variable (`pthread_cond_wait`) atomically releases the mutex while waiting, preventing busy-waiting
- The priority queue ensures fairness (FIFO or EDF ordering)
- When a coder releases a dongle (`put_dongles`), `pthread_cond_broadcast` wakes all waiting coders, and the next eligible one acquires it

### 2. **Logging Synchronization**

#### Problem
Multiple coder threads attempt to print state transitions simultaneously, causing interleaved/garbled output.

#### Solution: `coder_routine.c` - Log Mutex

```c
void print_state(t_coder *coder, const char *state)
{
    pthread_mutex_lock(&coder->sim->log_mutex);        /* Lock */
    pthread_mutex_lock(&coder->sim->stop_mutex);       /* Check stop flag */
    if (coder->sim->stop_flag == 0)
        printf("%lld %d %s\n", get_time_in_ms() - coder->sim->start_time_ms, 
               coder->id, state);
    pthread_mutex_unlock(&coder->sim->stop_mutex);
    pthread_mutex_unlock(&coder->sim->log_mutex);      /* Unlock */
}
```

**Race Condition Prevention**:
- The `log_mutex` ensures only one thread can print at a time
- The `stop_mutex` ensures the stop flag cannot change during the check
- All output is serialized → no garbled text

### 3. **Compile State & Counter Synchronization**

#### Problem
Multiple coders update their compile count and last_compile timestamp concurrently, causing:
- Lost updates
- Inconsistent state read by the monitor thread

#### Solution: `coder_routine.c` - Protected State Updates

```c
void update_compile_state(t_coder *coder)
{
    pthread_mutex_lock(&coder->sim->stop_mutex);
    coder->last_compile = get_time_in_ms();  /* Critical section */
    pthread_mutex_unlock(&coder->sim->stop_mutex);
}

void increment_compile_count(t_coder *coder)
{
    pthread_mutex_lock(&coder->sim->stop_mutex);
    coder->compile_count++;                   /* Critical section */
    pthread_mutex_unlock(&coder->sim->stop_mutex);
}
```

**Race Condition Prevention**:
- The `stop_mutex` protects all shared coder state
- Reads by the monitor thread are also protected
- Ensures atomicity of state updates

### 4. **Stop Flag Synchronization**

#### Problem
The monitor thread signals when the simulation should stop, but coders need to check this flag safely.

#### Solution: `codexion.h` - Dedicated Mutex + Flag

```c
struct s_sim {
    int              stop_flag;
    pthread_mutex_t  stop_mutex;  /* Protects stop_flag AND all coder state */
};
```

**Usage Pattern** (from `coder_routine.c`):

```c
while (1)
{
    pthread_mutex_lock(&coder->sim->stop_mutex);
    if (coder->sim->stop_flag == 1)  /* Atomic check */
    {
        pthread_mutex_unlock(&coder->sim->stop_mutex);
        break;
    }
    pthread_mutex_unlock(&coder->sim->stop_mutex);
    
    /* Perform work (compile, debug, refactor) */
}
```

**Race Condition Prevention**:
- The flag cannot change between check and action
- All coder state changes are synchronized with the same mutex

### 5. **Timer Synchronization (Phase 2)**

#### Problem
Original code used busy-waiting (`usleep` loops), consuming CPU cycles during idle time.

#### Solution: `time.c` - Interrupt-Driven Sleep with Absolute Timespec

```c
struct s_coder {
    pthread_mutex_t  time_mutex;  /* Dedicated to sleep lock */
    pthread_cond_t   time_cond;   /* Wakeup signal from OS */
};

void ft_smart_sleep(t_coder *coder, long long time_ms)
{
    struct timespec ts;
    
    if (time_ms <= 0)
        return;
    
    /* Calculate absolute wake time */
    get_target_timespec(&ts, time_ms);
    
    /* Lock, wait, unlock */
    pthread_mutex_lock(&coder->time_mutex);
    pthread_cond_timedwait(&coder->time_cond, &coder->time_mutex, &ts);
    pthread_mutex_unlock(&coder->time_mutex);
}
```

**Advantages**:
- **0% CPU usage** during sleep (thread yields to OS scheduler)
- Precise timing using absolute `struct timespec`
- Thread is woken up by kernel interrupt exactly at target time
- Replaces busy-waiting loops: `while ((get_time_in_ms() - start) < duration)`

**Race Condition Prevention**:
- Each coder has its own timer mutex and condition variable
- Independent sleep operations don't interfere with each other
- Atomic wait operation (mutex released while waiting, re-acquired on wakeup)

### 6. **Monitor Thread Synchronization**

#### Problem
The monitor thread must read shared state from multiple coders while they're modifying it, without causing:
- Inconsistent reads (partially updated state)
- Deadlocks (waiting on each other)

#### Solution: `monitor.c` - Protected Read Pattern

```c
int check_death(t_sim *sim, int i)
{
    int burnout;
    
    pthread_mutex_lock(&sim->stop_mutex);
    burnout = (get_time_in_ms() - sim->coders[i].last_compile) 
              > sim->params.time_to_burnout;
    pthread_mutex_unlock(&sim->stop_mutex);
    return (burnout);
}

int check_all_finished(t_sim *sim, int finished)
{
    int result;
    
    pthread_mutex_lock(&sim->stop_mutex);
    result = (finished == sim->params.num_coders);
    pthread_mutex_unlock(&sim->stop_mutex);
    return (result);
}
```

**Race Condition Prevention**:
- The monitor acquires the same `stop_mutex` used by coders
- Ensures consistent snapshot of state during checks
- Prevents reading partially-updated state
- No busy-waiting; monitor sleeps between checks

---

## Blocking Cases Handled

### 1. **Deadlock Prevention (Coffman's Conditions)**

Deadlock occurs when all four Coffman conditions are met:
1. **Mutual Exclusion** - Resources cannot be shared
2. **Hold and Wait** - Threads hold resources while waiting for others
3. **No Preemption** - Resources cannot be taken forcibly
4. **Circular Wait** - Threads wait in a circular dependency

**Solution: Break Circular Wait**

```c
/* In init_coders: Enforce consistent acquisition order */
if (coder->id % 2 == 0) {
    take_dongles(coder, coder->right_dongle);  /* Even: right first */
    take_dongles(coder, coder->left_dongle);
} else {
    take_dongles(coder, coder->left_dongle);   /* Odd: left first */
    take_dongles(coder, coder->right_dongle);
}
```

**Why it works**:
- Even coders always acquire `right → left`
- Odd coders always acquire `left → right`
- This prevents circular dependencies
- If all coders wait for the same resource sequence, no cycles form

### 2. **Starvation Prevention**

#### Problem
Coders might be continuously denied access to dongles if the scheduler is unfair.

#### Solution: Priority Queue + EDF Scheduling

```c
/* In take_dongles: Queue-based fairness */
wait_for_availability(dongle, coder->id)
/* Condition: pq_peek(&queue)->coder_id->id == my_id */
```

**Fairness guarantee**:
- **FIFO mode**: First coder in queue always gets the dongle
- **EDF mode**: Coder with earliest deadline (closest to burnout) gets priority
- No coder is indefinitely postponed
- Prevents starvation of low-priority coders

### 3. **Dongle Cooldown Handling**

#### Problem
If a dongle is immediately reused, fast coders might monopolize it, starving others.

#### Solution: Cooldown Period + Atomic Check

```c
static void wait_for_cooldown(t_dongle *dongle)
{
    while (get_time_in_ms() < dongle->cooldown_timestamp)
    {
        pthread_mutex_unlock(&dongle->mutex);
        usleep(50);                    /* Sleep without holding mutex */
        pthread_mutex_lock(&dongle->mutex);
    }
}
```

**Why it works**:
- After release, dongle is unavailable for `dongle_cooldown` ms
- This gives other waiting coders a chance to acquire it
- Prevents resource hogging
- Mutex is released during sleep to allow other coders to make progress

### 4. **Precise Burnout Detection**

#### Problem
Burnout check must be accurate and timely:
- If checked too infrequently, coders burn out late
- Race conditions between check and update of `last_compile`

#### Solution: Monitor Thread + Synchronized State

```c
int check_death(t_sim *sim, int i)
{
    long long elapsed;
    
    pthread_mutex_lock(&sim->stop_mutex);
    elapsed = get_time_in_ms() - sim->coders[i].last_compile;
    pthread_mutex_unlock(&sim->stop_mutex);
    
    if (elapsed > sim->params.time_to_burnout) {
        print_state(&sim->coders[i], "is burned out");
        return (1);
    }
    return (0);
}
```

**Accuracy guarantees**:
- Monitor checks every coder's compile time regularly
- `last_compile` update is atomic (protected by mutex)
- No TOCTOU (Time-of-Check-Time-of-Use) race condition
- Burnout timestamp is always reliable

### 5. **Log Serialization**

#### Problem
Concurrent printing from multiple threads causes:
```
0 1 is0 2 compiling
has taken a dongle
```

#### Solution: Log Mutex in print_state

```c
void print_state(t_coder *coder, const char *state)
{
    pthread_mutex_lock(&coder->sim->log_mutex);       /* Serialize all output */
    pthread_mutex_lock(&coder->sim->stop_mutex);
    if (coder->sim->stop_flag == 0)
        printf("%lld %d %s\n", 
               get_time_in_ms() - coder->sim->start_time_ms,
               coder->id, state);
    pthread_mutex_unlock(&coder->sim->stop_mutex);
    pthread_mutex_unlock(&coder->sim->log_mutex);
}
```

**Guarantees**:
- Each `printf` call completes atomically
- Output is always well-formed and readable
- No interleaving of multiple threads' output

### 6. **Simulation Termination Safety**

#### Problem
How do we signal all threads to stop without causing races?

#### Solution: `stop_flag` + `pthread_join`

```c
/* In codexion.c: main thread */
monitor_routine(&sim);      /* Wait until game ends */
join_sim(&sim);             /* Join all coder threads */
cleanup_sim(&sim);          /* Destroy all synchronization primitives */
```

**Implementation details**:
- Monitor sets `stop_flag = 1` when game ends
- Each coder checks `stop_flag` in its main loop
- Main thread joins all coder threads (waits for them to exit)
- Only then does cleanup occur (no use-after-free)

---

## Resources

### Documentation & References

- **POSIX Threads (pthread)**
  - https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html
  - `pthread_mutex_t`, `pthread_cond_t`, `pthread_cond_timedwait` documentation

- **Dining Philosophers Problem**
  - Classic computer science problem demonstrating deadlock and starvation
  - https://en.wikipedia.org/wiki/Dining_philosophers_problem

- **Deadlock Prevention (Coffman's Conditions)**
  - https://en.wikipedia.org/wiki/Deadlock#Necessary_and_sufficient_conditions
  - Resource ordering technique to prevent circular wait

- **Priority Queues & Scheduling**
  - FIFO (First-In-First-Out) vs EDF (Earliest Deadline First)
  - https://en.wikipedia.org/wiki/Real-time_computing#Scheduling

- **42 School Norm**
  - Maximum 25 lines per function
  - No global variables
  - Specific formatting and style requirements

### AI Assistance Used

**Claude (AI Assistant)** was utilized for the following tasks:

1. **Phase 2 Implementation (Interrupt-Driven Timing Engine)**
   - Designed the `get_target_timespec()` function for absolute time calculation
   - Implemented `ft_smart_sleep()` wrapper using `pthread_cond_timedwait`
   - Added proper nanosecond carry-over arithmetic
   - Lifecycle management (mutex/condition variable initialization and cleanup)

2. **42 School Norm Refactoring**
   - Analyzed function lengths and identified violations (>25 lines)
   - Extracted helper functions to reduce complexity:
     - `take_both_dongles_even()`, `take_both_dongles_odd()`
     - `put_both_dongles_even()`, `put_both_dongles_odd()`
     - `update_compile_state()`, `increment_compile_count()`
     - `push_request_to_queue()`, `wait_for_availability()`, `wait_for_cooldown()`
   - Verified compliance post-refactoring

3. **README.md Creation**
   - Comprehensive documentation of threading mechanisms
   - Detailed deadlock and starvation prevention explanations
   - Technical architecture and synchronization patterns
   - Usage examples and parameter descriptions

4. **Code Organization**
   - Identified and removed unused files (src/ directory)
   - Verified compilation and runtime correctness after changes

---

## Building & Running

### Complete Workflow

```bash
# Navigate to project root
cd /home/tlaghzal/Codexion-42

# Go to coders directory
cd coders

# Build the project
make              # or make codexion

# Run a test simulation
./codexion 3 1000 100 100 100 1 100 fifo

# Clean up
make clean        # Remove .o files
make fclean       # Remove .o and binary
make re           # Clean and rebuild
```

### Expected Output

```
Parsed: 3 coders, scheduler enum: 0 (0=FIFO, 1=EDF)
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
100 1 is debugging
200 1 is refactoring
300 1 has taken a dongle
...
```

---

## Project Structure

```
/home/tlaghzal/Codexion-42/
├── README.md                 # This file
├── coders/
│   ├── Makefile             # Build configuration
│   ├── codexion.h           # Main header (structures, prototypes)
│   ├── codexion.c           # Main entry point
│   ├── init.c               # Initialization functions
│   ├── coder_routine.c      # Coder thread main loop
│   ├── dongle.c             # Dongle acquisition/release
│   ├── simulation.c         # Simulation control
│   ├── monitor.c            # Monitor thread
│   ├── time.c               # Timing functions (Phase 2)
│   ├── pq.c                 # Priority queue implementation
│   ├── utils.c              # Utility functions
│   └── garbge_colle/        # Custom memory allocator
│       ├── ft_malloc.c
│       └── ft_malloc.h
```

---

**Author**: tlaghzal  
**School**: 42 Khouribga  
**Last Updated**: April 12, 2026
