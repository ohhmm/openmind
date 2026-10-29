#!/bin/bash

DIR="${1:-.}"

INPUT="Hi"
if [ "$#" -ne 0 ]; then
    INPUT=$@
fi

cd $DIR &&
    ollama launch claude --model beam:cloud -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence --allowed-tools "*" -p "${INPUT}"
