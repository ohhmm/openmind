#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if (( $# > 0 )); then
   echo "Arguments were provided:"
   printf '  %s\n' "$@"
   PARAMS=origin/$@
else
   echo "No arguments were provided, rebasing onto origin/main"
fi


cd "$SCRIPT_DIR"
git fetch --prune --tags
git rebase --autostash -i origin/main $PARAMS
