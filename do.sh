#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT="Hi"
MODEL="${OLLAMA_MODEL:-beam:cloud}"
FALLBACK_MODEL="${OLLAMA_FALLBACK_MODEL:-qwen3.5:9b}"

if [ "$#" -ne 0 ]; then
    INPUT="$*"
fi

if ! "$SCRIPT_DIR/ollama.sh" run "$MODEL" hi >/dev/null 2>&1; then
    echo "Primary model unavailable; falling back to $FALLBACK_MODEL" >&2
    "$SCRIPT_DIR/ollama.sh" run "$FALLBACK_MODEL" hi >/dev/null 2>&1
    MODEL="$FALLBACK_MODEL"
fi

cd "$SCRIPT_DIR" &&
    ./caffeinate.sh ./ollama.sh launch --yes claude --model "$MODEL" -- --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
