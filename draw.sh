#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

INPUT_FILE="${1:-$SCRIPT_DIR/input.txt}"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/build}"
CONFIGURATION="${CONFIGURATION:-Release}"
TARGET="draw-pattern-relations"

if [ ! -e "$BUILD_DIR/CMakeCache.txt" ]; then
   "$SCRIPT_DIR/cmake.sh" -S "$SCRIPT_DIR" -B "$BUILD_DIR"
fi

"$SCRIPT_DIR/cmake.sh" --build "$BUILD_DIR" --config "$CONFIGURATION" --target "$TARGET"

EXE="$BUILD_DIR/bin/$CONFIGURATION/$TARGET"
if [ ! -x "$EXE" ]; then
    EXE="$BUILD_DIR/bin/$TARGET"
fi
exec "$EXE" < "$INPUT_FILE"
