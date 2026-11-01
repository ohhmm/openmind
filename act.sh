#!/bin/bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

REQUEST="continue thinking.log on consistent successful mergable resolve of task.md till branch passed CI"
REQUEST="$REQUEST $*"

printf '%s\n' "$SCRIPT_DIR/update.sh"
"$SCRIPT_DIR/update.sh" &&
    "$SCRIPT_DIR/do.sh" "$REQUEST" >> "$SCRIPT_DIR/thinking.log" &&
    git -C "$SCRIPT_DIR" diff &&
    git -C "$SCRIPT_DIR" status &&
    git -C "$SCRIPT_DIR" show
