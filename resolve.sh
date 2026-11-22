#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

unmerged=$(git ls-files --unmerged)
if [[ -n "$unmerged" ]]; then
   bash -n ./do.sh
   ./do.sh 'Resolve the Git conflicts in this working copy.
Use git status and git ls-files --unmerged to identify conflicted paths.
Inspect the index stages and the current working copy. Preserve compatible
changes from both sides, existing manual resolutions, and unrelated changes.
Stage only the resolved paths. Report any conflicts you cannot resolve
confidently and leave them unmerged. Leave the current Git operation paused
without committing, pushing, continuing, or aborting it.'
   unmerged=$(git ls-files --unmerged)
   if [[ -n "$unmerged" ]]; then
      echo "Unresolved conflicts remain." >&2
      exit 1
   fi
fi
