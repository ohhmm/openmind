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

if ! ollama run "$MODEL" hi >/dev/null 2>&1; then
    echo "Primary model unavailable; falling back to $FALLBACK_MODEL" >&2
    ollama pull "$FALLBACK_MODEL"
    ollama run "$FALLBACK_MODEL" hi >/dev/null 2>&1
    MODEL="$FALLBACK_MODEL"
fi

cd "$SCRIPT_DIR" &&
    "${CAFFEINATE[@]}" ollama launch --yes claude --model "$MODEL" -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"