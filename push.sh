#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if (( $# > 0 )); then
   echo "Arguments were provided:"
   printf '  %s\n' "$@"
   PARAMS=HEAD:$@
else
   echo "No arguments were provided, not force-pushing. Would you like:"
   echo ./harness.sh /pre-push
   echo git push origin --force-with-lease origin $PARAMS
   exit 1
fi

exec git -C "$SCRIPT_DIR" push origin --force-with-lease $PARAMS
   