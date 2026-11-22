#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if (( $# > 0 )); then
   echo "Arguments were provided:"
   printf '  %s\n' "$@"
   if [[ "$@" == -* ]]; then
      PARAMS=$@
   else
      PARAMS=origin/$@
   fi
else
   echo "No arguments were provided, rebasing onto origin/main"
fi


cd "$SCRIPT_DIR" &&
   fetch --prune --tags
cd "$SCRIPT_DIR" &&
   exec git rebase --autostash -i origin/main $PARAMS
