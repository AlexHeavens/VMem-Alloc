#!/bin/bash
# Lint the workflows (actionlint) and the shell scripts (ShellCheck) with the
# image versions pinned in pins/. CI runs the same command. Needs docker.
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

pinned_image() {
    sed -nE 's/^FROM (.+)$/\1/p' "$REPO_DIR/pins/$1/Dockerfile"
}

actionlint=$(pinned_image actionlint)
shellcheck=$(pinned_image shellcheck)
[ -n "$actionlint" ] && [ -n "$shellcheck" ] || { echo "missing image pin in pins/" >&2; exit 1; }

docker run --rm --volume "$REPO_DIR:/repo" --workdir /repo "$actionlint" -color
docker run --rm --volume "$REPO_DIR:/repo" --workdir /repo "$shellcheck" \
    scripts/*.sh .githooks/pre-commit
echo "lint passed"
