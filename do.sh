#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT="Hi"
MODEL="${OLLAMA_MODEL:-beam:cloud}"
FALLBACK_MODEL="${OLLAMA_FALLBACK_MODEL:-qwen3.5:9b}"
OLLAMA_BIN="ollama"

if ! command -v "$OLLAMA_BIN" >/dev/null 2>&1; then
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*)
            if ! command -v winget >/dev/null 2>&1; then
                echo "Ollama is not installed and winget is unavailable." >&2
                exit 1
            fi
            if ! winget install --id Ollama.Ollama --exact --accept-source-agreements --accept-package-agreements; then
                echo "Failed to install Ollama with winget." >&2
                exit 1
            fi
            if ! command -v "$OLLAMA_BIN" >/dev/null 2>&1 && command -v cygpath >/dev/null 2>&1 && [ -n "${LOCALAPPDATA:-}" ]; then
                WINDOWS_OLLAMA="$(cygpath -u "$LOCALAPPDATA")/Programs/Ollama/ollama.exe"
                if [ -x "$WINDOWS_OLLAMA" ]; then
                    OLLAMA_BIN="$WINDOWS_OLLAMA"
                fi
            fi
            ;;
        Darwin|Linux)
            if ! command -v curl >/dev/null 2>&1; then
                echo "Ollama is not installed and curl is unavailable." >&2
                exit 1
            fi
            if ! curl -fsSL https://ollama.com/install.sh | sh; then
                echo "Failed to install Ollama with the official install script." >&2
                exit 1
            fi
            ;;
        *)
            echo "Ollama is not installed; automatic installation is unsupported on $(uname -s)." >&2
            exit 1
            ;;
    esac

    if ! command -v "$OLLAMA_BIN" >/dev/null 2>&1 && [ "$OLLAMA_BIN" = "ollama" ]; then
        echo "Ollama was installed, but its command is not available in PATH. Restart the shell and try again." >&2
        exit 1
    fi
fi

if [ "$#" -ne 0 ]; then
    INPUT="$*"
fi

CAFFEINATE=()
if command -v caffeinate >/dev/null 2>&1; then
    CAFFEINATE=(caffeinate -i)
fi

if ! "$OLLAMA_BIN" run "$MODEL" hi >/dev/null 2>&1; then
    echo "Primary model unavailable; falling back to $FALLBACK_MODEL" >&2
    "$OLLAMA_BIN" pull "$FALLBACK_MODEL"
    "$OLLAMA_BIN" run "$FALLBACK_MODEL" hi >/dev/null 2>&1
    MODEL="$FALLBACK_MODEL"
fi

cd "$SCRIPT_DIR" &&
    "${CAFFEINATE[@]}" "$OLLAMA_BIN" launch --yes claude --model "$MODEL" -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
