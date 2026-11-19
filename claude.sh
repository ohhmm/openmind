#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CLAUDE_CMD=claude
CLAUDE_HOME="$HOME"
CLAUDE_BINARY=claude
if ! command -v "$CLAUDE_CMD" >/dev/null 2>&1; then
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*)
            CLAUDE_HOME="$(cygpath -u "$USERPROFILE")" || exit 1
            CLAUDE_BINARY=claude.exe
            ;;
    esac

    CLAUDE_CMD="$CLAUDE_HOME/.local/bin/$CLAUDE_BINARY"
    if [ ! -x "$CLAUDE_CMD" ] && [ ! -x "$CLAUDE_HOME/.claude/local/$CLAUDE_BINARY" ]; then
        # Ollama installs the integration before running the version check.
        "$SCRIPT_DIR/ollama.sh" launch --yes claude --model "${MODEL:-${OLLAMA_MODEL:-gemma4:cloud}}" -- --version >&2 || exit 1
    fi
    if [ ! -x "$CLAUDE_CMD" ]; then
        CLAUDE_CMD="$CLAUDE_HOME/.claude/local/$CLAUDE_BINARY"
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
