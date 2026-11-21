# VMem-Alloc

- Build: `make -j2`. Test: `make test`. Sanitizers: `make clean && make test EXTRA_CFLAGS="-O1 -fsanitize=address,undefined"`.
- Lint: `scripts/lint.sh` (actionlint and ShellCheck in pinned docker images). The C code builds with `-Wall -Wextra -Werror`.
- Docs: `README.md` for setup and contents.
- Before each commit, check the staged diff for secrets. The pre-commit hook needs `git config core.hooksPath .githooks`.
