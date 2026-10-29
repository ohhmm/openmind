#!/bin/bash
SCRIPT_DIR=$(dirname "$0")
INPUT="Hi"

if [ "$#" -ne 0 ]; then
    INPUT=$@
fi
echo "$INPUT" > task.md
echo $INPUT
echo $SCRIPT_DIR/act.sh
$SCRIPT_DIR/act.sh
