#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODEL=Beam-1.0-501B-A23B
MIRROR_CMD=mirror

if ! command -v "$MIRROR_CMD" >/dev/null 2>&1; then
    if ! command -v uv >/dev/null 2>&1; then
        curl -LsSf https://astral.sh/uv/install.sh | sh
        export PATH="$HOME/.local/bin:$PATH"
    fi
    uv tool install reflection-mirror
    MIRROR_CMD="${UV_TOOL_BIN_DIR:-$HOME/.local/bin}/mirror"
fi

cd "$SCRIPT_DIR" &&
    exec "$MIRROR_CMD" --api reflection --model "$MODEL" "$@"
