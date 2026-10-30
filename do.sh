#!/bin/bash
SCRIPT_DIR=$(dirname "$0")
INPUT="Hi"

if [ "$#" -ne 0 ]; then
    INPUT=$@
fi

cd "$SCRIPT_DIR" &&
    ollama launch --yes claude --model beam:cloud -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
