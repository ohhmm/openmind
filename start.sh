#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 0 ]; then
    INPUT="$*"
    printf '%s\n' "$INPUT" > "$SCRIPT_DIR/task.md"
else
    if [ ! -f "$SCRIPT_DIR/task.md" ]; then
        printf 'task.md not found: %s\n' "$SCRIPT_DIR/task.md" >&2
        INPUT="Do your thing"
        printf '%s\n' "$INPUT" > "$SCRIPT_DIR/task.md"
    else
        printf '%s\n' "$SCRIPT_DIR/task.md will be used"
        INPUT="$(cat "$SCRIPT_DIR/task.md")"
    fi
fi
printf 'Task taken: %s\n' "$INPUT"
cp "$SCRIPT_DIR/task.md" "$SCRIPT_DIR/thinking.log"

printf '%s\n' "$SCRIPT_DIR/act.sh"
"$SCRIPT_DIR/act.sh" $@
