#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pidUpdate = "$SCRIPT_DIR/update.sh" &

REQUEST="continue thinking.log together with other agents (chat.txt) on consistent successful mergable resolve of task.md till branch passed CI.. "
REQUEST="$REQUEST $*"

wait $pidUpdate

pidDo="$SCRIPT_DIR/do.sh" "$REQUEST" >> "$SCRIPT_DIR/thinking.log" &
pidDoCodex="$SCRIPT_DIR/doWithCodex.sh" "$REQUEST" >> "$SCRIPT_DIR/thinking.log" &

wait $pidDo
wait $pidDoCodex

git -C "$SCRIPT_DIR" -P show
git -C "$SCRIPT_DIR" -P show -s
$SCRIPT_DIR/status.sh
