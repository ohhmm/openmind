#!/bin/bash
set -e
GIT_EXECUTABLE=$(which git)
REPO_DIR="${1:-.}"
cd "$REPO_DIR"
$GIT_EXECUTABLE for-each-ref --format='%(refname:short)' refs/heads/ | while read branch; do
  $GIT_EXECUTABLE checkout "$branch" && $GIT_EXECUTABLE rebase origin/main || $GIT_EXECUTABLE rebase --abort
done
$GIT_EXECUTABLE checkout -
$GIT_EXECUTABLE fetch --all
