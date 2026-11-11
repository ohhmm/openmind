#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CODEX_CMD=codex

if ! command -v "$CODEX_CMD" >/dev/null 2>&1; then
    curl -fsSL https://chatgpt.com/codex/install.sh | CODEX_NON_INTERACTIVE=1 sh
    CODEX_CMD="${CODEX_INSTALL_DIR:-$HOME/.local/bin}/codex"
fi

cd "$SCRIPT_DIR" &&
    exec "$CODEX_CMD" --no-daemon "$@"
