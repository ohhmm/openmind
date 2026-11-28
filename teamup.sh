#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"$SCRIPT_DIR/update.sh" &
pidUpdate=$!

REQUEST="continue thinking.log together with other agents (monitor chat.txt) on consistent successful mergable resolve of task.md till branch passed CI.. "
REQUEST="$REQUEST $*"
touch "$SCRIPT_DIR/thinking.log"
touch "$SCRIPT_DIR/chat.txt"

wait $pidUpdate && echo "update.sh finished successfully" || echo "update.sh failed" > "$SCRIPT_DIR/actogether.log"

"$SCRIPT_DIR/do.sh" "$REQUEST" >> "$SCRIPT_DIR/thinking.log" &
pidDo=$!
"$SCRIPT_DIR/do-with-codex.sh" "$REQUEST" >> "$SCRIPT_DIR/thinking.log" &
pidDoCodex=$!

wait $pidDo && echo "do.sh finished successfully" || echo "do.sh failed" >> "$SCRIPT_DIR/actogether.log"
wait $pidDoCodex && echo "do-with-codex.sh finished successfully" || echo "do-with-codex.sh failed" >> "$SCRIPT_DIR/actogether.log"

git -C "$SCRIPT_DIR" -P show
git -C "$SCRIPT_DIR" -P show -s
$SCRIPT_DIR/status.sh
