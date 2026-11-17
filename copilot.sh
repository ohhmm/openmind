#!/bin/bash

set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OS="$(uname -s 2>/dev/null || printf 'Unknown')"
COPILOT_PREFIX="${COPILOT_PREFIX:-$HOME/.local}"
COPILOT_CMD=copilot

resolve_copilot() {
    if command -v "$COPILOT_CMD" >/dev/null 2>&1; then
        return 0
    fi

    case "$OS" in
        MINGW*|MSYS*|CYGWIN*)
            if [ -n "${LOCALAPPDATA:-}" ] && command -v cygpath >/dev/null 2>&1; then
                COPILOT_PATH="$(cygpath -u "$LOCALAPPDATA/GitHubCopilotCLI/copilot.exe")" || return 1
                if [ -f "$COPILOT_PATH" ]; then
                    COPILOT_CMD="$COPILOT_PATH"
                    return 0
                fi
            fi
            ;;
        Darwin*|Linux*)
            COPILOT_PATH="$COPILOT_PREFIX/bin/copilot"
            if [ -x "$COPILOT_PATH" ]; then
                COPILOT_CMD="$COPILOT_PATH"
                return 0
            fi
            ;;
    esac

    return 1
}

if ! resolve_copilot; then
    case "$OS" in
        MINGW*|MSYS*|CYGWIN*)
            if command -v winget >/dev/null 2>&1; then
                winget install --id GitHub.Copilot --exact --accept-source-agreements --accept-package-agreements
            elif command -v npm >/dev/null 2>&1; then
                npm install -g @github/copilot
            else
                printf 'Windows Copilot installation requires WinGet or npm.\n' >&2
                exit 1
            fi
            ;;
        Darwin*|Linux*)
            COPILOT_BIN="$COPILOT_PREFIX/bin"
            if command -v curl >/dev/null 2>&1; then
                curl -fsSL https://gh.io/copilot-install | PREFIX="$COPILOT_PREFIX" PATH="$COPILOT_BIN:$PATH" bash
                COPILOT_CMD="$COPILOT_BIN/copilot"
            elif command -v wget >/dev/null 2>&1; then
                wget -qO- https://gh.io/copilot-install | PREFIX="$COPILOT_PREFIX" PATH="$COPILOT_BIN:$PATH" bash
                COPILOT_CMD="$COPILOT_BIN/copilot"
            elif command -v brew >/dev/null 2>&1; then
                brew install --cask copilot-cli
            elif command -v npm >/dev/null 2>&1; then
                npm install -g @github/copilot
            else
                printf 'Copilot installation requires curl, wget, Homebrew, or npm.\n' >&2
                exit 1
            fi
            ;;
        *)
            printf 'Unsupported operating system: %s\n' "$OS" >&2
            exit 1
            ;;
    esac
fi

if ! resolve_copilot; then
    printf 'Copilot executable not found: %s\n' "$COPILOT_CMD" >&2
    exit 1
fi

cd "$SCRIPT_DIR" &&
    exec "$COPILOT_CMD" "$@"
