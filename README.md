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

## For Your Portfolio

This project is suitable for showcasing:

- Systems-level C programming and memory management.
- Debugging and monitoring techniques for runtime behavior.
- Clear modular code organization and build automation with `Makefile`.

If you'd like, I can add a short demo script, example output, or a small
section showing typical allocator traces to make this README even more
portfolio-ready.

---

Maintained by the author.
