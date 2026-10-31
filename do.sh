#!/bin/bash
SCRIPT_DIR=$(dirname "$0")
INPUT="Hi"
MODEL="${OLLAMA_MODEL:-beam:cloud}"

if [ "$#" -ne 0 ]; then
    INPUT=$@
fi

CAFFEINATE=()
if command -v caffeinate >/dev/null 2>&1; then
    CAFFEINATE=(caffeinate -i)
fi

cd "$SCRIPT_DIR" &&
    "${CAFFEINATE[@]}" ollama launch --yes claude --model "$MODEL" -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
