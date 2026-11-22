#!/bin/bash
# Install the gitleaks release pinned in pins/gitleaks/Dockerfile into a directory
# (default ~/.local/bin), after checking the archive against the release's checksum file.
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEST="${1:-$HOME/.local/bin}"

version=$(sed -nE 's/^FROM zricethezav\/gitleaks:v([0-9]+\.[0-9]+\.[0-9]+)$/\1/p' "$REPO_DIR/pins/gitleaks/Dockerfile")
[ -n "$version" ] || { echo "no gitleaks version in pins/gitleaks/Dockerfile" >&2; exit 1; }

case "$(uname -s)-$(uname -m)" in
    Linux-x86_64)  platform=linux_x64 ;;
    Linux-aarch64) platform=linux_arm64 ;;
    Darwin-x86_64) platform=darwin_x64 ;;
    Darwin-arm64)  platform=darwin_arm64 ;;
    *) echo "no gitleaks release for $(uname -s) $(uname -m)" >&2; exit 1 ;;
esac

archive="gitleaks_${version}_${platform}.tar.gz"
base="https://github.com/gitleaks/gitleaks/releases/download/v${version}"
workdir=$(mktemp -d)
trap 'rm -rf "$workdir"' EXIT

curl --fail --silent --show-error --location --output "$workdir/$archive" "$base/$archive"
curl --fail --silent --show-error --location --output "$workdir/checksums.txt" "$base/gitleaks_${version}_checksums.txt"
(cd "$workdir" && grep " ${archive}\$" checksums.txt | { sha256sum --check - 2>/dev/null || shasum --algorithm 256 --check -; })
tar --extract --gzip --file "$workdir/$archive" --directory "$workdir" gitleaks
mkdir -p "$DEST"
install -m 755 "$workdir/gitleaks" "$DEST/gitleaks"
echo "installed gitleaks $version to $DEST/gitleaks"
