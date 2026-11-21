# VMem-Alloc

A small C virtual memory allocation library (`vmemalloc`, `vmemfree`) built on `mmap`, with a free list, block merging and optional trace output. It is a coursework-style teaching project. It is not a drop-in replacement for `malloc` and is not production hardened.

## Contents

- `vmemalloc.c`, `vmemalloc.h`: the allocator and its public interface.
- `vmemprint.c`, `vmemprint.h`, `vmemaccess.h`: printing of the heap and free list, and the shared internals it reads.
- `vmemtest.c`: helper functions for the tests.
- `test1.c`, `testSetA.c`, `testSetB.c`, `testSetC.c`: test programs (basic, standard, stress and edge cases).
- `test data/`: trace output and graphs from earlier runs.
- `Makefile`: builds the library into `lib/` and the test programs into `bin/`.
- `scripts/`, `pins/`, `.githooks/`: secrets scanning, linting and tool version pins.

## Setup

Prerequisites: gcc and make, and docker for the linters.

```
scripts/install-gitleaks.sh        # the pre-commit hook needs gitleaks
git config core.hooksPath .githooks
make -j2
```

## Tests

```
make test
scripts/lint.sh
```

`make test` runs the four test programs in `build/run`, so the tracked files in `test data/` stay unchanged. A program that fails exits non-zero and the make run fails. CI also runs `make test` under AddressSanitizer and UndefinedBehaviorSanitizer:

```
make clean && make test EXTRA_CFLAGS="-O1 -fsanitize=address,undefined"
```
