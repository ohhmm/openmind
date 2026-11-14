#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

INPUT_FILE="${1:-$SCRIPT_DIR/input.txt}"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/build}"
CONFIGURATION="${CONFIGURATION:-Release}"
TARGET="draw-pattern-relations"

if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    "$SCRIPT_DIR/cmake.sh" -S "$SCRIPT_DIR" -B "$BUILD_DIR"
fi

"$SCRIPT_DIR/cmake.sh" --build "$BUILD_DIR" --config $CONFIGURATION --target "$TARGET"

exec "$BUILD_DIR/bin/$CONFIGURATION/$TARGET" < "$INPUT_FILE"
