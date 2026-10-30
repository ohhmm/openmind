#!/bin/bash
SCRIPT_DIR=$(dirname "$0")
INPUT="Hi"

if [ "$#" -ne 0 ]; then
    INPUT=$@
fi
cd $SCRIPT_DIR
echo "$INPUT" > $SCRIPT_DIR/task.md
echo "$INPUT" > $SCRIPT_DIR/thinking.log
echo $SCRIPT_DIR/act.sh
$SCRIPT_DIR/act.sh
