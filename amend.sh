#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    REPO_DIR="$(git rev-parse --show-toplevel)"
else
    REPO_DIR="$SCRIPT_DIR"
fi

cd "$REPO_DIR"

if ! git rev-parse --verify HEAD >/dev/null 2>&1; then
    echo "amend.sh: no commit to amend" >&2
    exit 1
fi

# Amend HEAD using only changes already staged in the index.
# Unstaged working-tree changes are left untouched, and the commit message is preserved.
exec git commit --amend --no-edit
