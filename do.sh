#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT="Hi"
MODEL="${OLLAMA_MODEL:-beam:cloud}"
FALLBACK_MODEL="${OLLAMA_FALLBACK_MODEL:-qwen3.5:9b}"

if [ "$#" -ne 0 ]; then
    INPUT="$*"
fi

CAFFEINATE=()
if command -v caffeinate >/dev/null 2>&1; then
    CAFFEINATE=(caffeinate -i)
fi

OLLAMA_CMD=ollama
ensure_ollama() {
    if command -v ollama >/dev/null 2>&1; then
        OLLAMA_CMD=ollama
        return 0
    fi

    # Predetect already-installed app bundle locations before attempting install.
    for candidate in \
        "/Applications/Ollama.app/Contents//Resources/ollama" \
        "/Applications/Ollama.app/Contents/Resources/ollama"; do
        if [ -n "$candidate" ] && [ -x "$candidate" ]; then
            OLLAMA_CMD="$candidate"
            return 0
        fi
    done

    case "$(uname -s 2>/dev/null || printf 'Unknown')" in
        Linux|Darwin)
            curl -fsSL https://ollama.com/install.sh | sh
            ;;
        MINGW*|MSYS*|CYGWIN*)
            if command -v winget >/dev/null 2>&1; then
                winget install --id Ollama.Ollama --exact --accept-source-agreements --accept-package-agreements
            else
                powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "irm https://ollama.com/install.ps1 | iex"
            fi
            ;;
        *)
            if command -v winget >/dev/null 2>&1; then
                winget install --id Ollama.Ollama --exact --accept-source-agreements --accept-package-agreements
            elif command -v powershell.exe >/dev/null 2>&1; then
                powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "irm https://ollama.com/install.ps1 | iex"
            else
                curl -fsSL https://ollama.com/install.sh | sh
            fi
            ;;
    esac

    hash -r 2>/dev/null || true
    if command -v ollama >/dev/null 2>&1; then
        OLLAMA_CMD=ollama
        return 0
    fi

    for candidate in \
        "/Applications/Ollama.app/Contents//Resources/ollama" \
        "/Applications/Ollama.app/Contents/Resources/ollama" \
        "${LOCALAPPDATA:-}/Programs/Ollama/ollama.exe" \
        "${USERPROFILE:-}/AppData/Local/Programs/Ollama/ollama.exe" \
        "/c/Program Files/Ollama/ollama.exe" \
        "/c/Program Files (x86)/Ollama/ollama.exe"
    do
        if [ -n "$candidate" ] && [ -x "$candidate" ]; then
            OLLAMA_CMD="$candidate"
            return 0
        fi
    done

    echo "Failed to install or locate ollama" >&2
    exit 1
}
ensure_ollama
ollama() { "$OLLAMA_CMD" "$@"; }

if ! ollama run "$MODEL" hi >/dev/null 2>&1; then
    echo "Primary model unavailable; falling back to $FALLBACK_MODEL" >&2
    ollama pull "$FALLBACK_MODEL"
    ollama run "$FALLBACK_MODEL" hi >/dev/null 2>&1
    MODEL="$FALLBACK_MODEL"
fi

cd "$SCRIPT_DIR" &&
    "${CAFFEINATE[@]}" ollama launch --yes claude --model "$MODEL" -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"