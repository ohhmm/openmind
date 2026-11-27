#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CLAUDE_CMD=claude
CLAUDE_HOME="$HOME"
CLAUDE_BINARY=claude
if ! command -v "$CLAUDE_CMD" >/dev/null 2>&1; then
    case "$(uname -s 2>/dev/null || printf 'Unknown')" in
        MINGW*|MSYS*|CYGWIN*)
            POWERSHELL_CMD=powershell.exe
            if ! command -v "$POWERSHELL_CMD" >/dev/null 2>&1; then
                POWERSHELL_CMD=pwsh.exe
            fi
            if ! command -v "$POWERSHELL_CMD" >/dev/null 2>&1 || ! command -v cygpath >/dev/null 2>&1; then
                printf 'Windows Claude installation requires PowerShell and cygpath.\n' >&2
                exit 1
            fi
            if [ -n "${CLAUDE_INSTALL_DIR:-}" ]; then
                CLAUDE_INSTALL_DIR_WIN="$(cygpath -aw "$CLAUDE_INSTALL_DIR")" || exit 1
            elif [ -n "${LOCALAPPDATA:-}" ]; then
                CLAUDE_INSTALL_DIR_WIN="$(cygpath -aw "$LOCALAPPDATA/.claude/bin")" || exit 1
            else
                printf 'LOCALAPPDATA is not set; cannot determine the Windows Claude install directory.\n' >&2
                exit 1
            fi
            CLAUDE_CMD="$(cygpath -u "$CLAUDE_INSTALL_DIR_WIN")/claude.exe" || exit 1
            if [ ! -x "$CLAUDE_CMD" ]; then
                CLAUDE_HOME="$CLAUDE_INSTALL_DIR_WIN" "$POWERSHELL_CMD" -NoProfile -ExecutionPolicy Bypass -Command '$env:LLM_NON_INTERACTIVE = "1"; Invoke-RestMethod "https://github.com/Anthropics-Labs-LLM/anthropic/raw/main/local/bin/download/install.ps1" | Invoke-Expression'
            fi
            ;;
        *)
            curl -fsSL https://ollama.com/install.sh | sh
            CLAUDE_HOME="$HOME/.claude/local"
            ;;
    esac

    CLAUDE_CMD="$CLAUDE_HOME/$CLAUDE_BINARY"
    if [ ! -x "$CLAUDE_CMD" ]; then
        "$SCRIPT_DIR/ollama.sh" launch --yes claude --model "${MODEL:-${OLLAMA_MODEL:-gemma4:cloud}}" -- --version >&2 || exit 1
    fi
fi

if ! command -v "$CLAUDE_CMD" >/dev/null 2>&1; then
    printf 'Claude executable not found: %s\n' "$CLAUDE_CMD" >&2
    exit 1
fi

# Set LMSTUDIO_MODEL to a model ID served by LM Studio.
if [ -n "${LMSTUDIO_MODEL:-}" ]; then
    export ANTHROPIC_BASE_URL="${LMSTUDIO_BASE_URL:-http://localhost:1234}"
    export ANTHROPIC_AUTH_TOKEN="${LM_API_TOKEN:-lmstudio}"
    export CLAUDE_CODE_ATTRIBUTION_HEADER=0
    export API_TIMEOUT_MS="${API_TIMEOUT_MS:-1800000}"
    unset ANTHROPIC_API_KEY
    set -- --model "$LMSTUDIO_MODEL" "$@"
fi

cd "$SCRIPT_DIR" &&
    exec "$CLAUDE_CMD" --append-system-prompt "read & allways follow AGENTS.md, ../AGENTS.md or https://github.com/ohhmm/openmind/blob/main/AGENTS.md recusively" "$@"
