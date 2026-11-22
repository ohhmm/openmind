#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 0 ]; then
   INPUT="$*"
else
   INPUT=Hi
fi

cd "$SCRIPT_DIR" &&
   time ./codex.sh --yes --exec "$INPUT" --dangerously-bypass-approvals-and-sandbox --ephemeral
