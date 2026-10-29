#!/bin/bash

DIR="${1:-.}"

INPUT="Hi"
if [ "$#" -ne 0 ]; then
    INPUT=$@
fi

echo "$INPUT" > task.md

$DIR/act.sh
