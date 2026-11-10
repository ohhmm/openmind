#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT=--

# OLLAMA_MODELS is a space- or comma-separated list of models in order of
# preference. Legacy OLLAMA_MODEL / OLLAMA_FALLBACK_MODEL inputs are still
# accepted and folded into the same list.
# OLLAMA_MODELS="${OLLAMA_MODELS:-beam:cloud qwen3.5:9b qwen3.5:4b}"
# OLLAMA_MODELS=beam:cloud qwen3.5:9b qwen3.5:4b
if [ -n "${OLLAMA_MODELS:-}" ]; then
    :
elif [ -n "${OLLAMA_MODEL:-}" ]; then
    OLLAMA_MODELS="$OLLAMA_MODEL"
    [ -n "${OLLAMA_FALLBACK_MODEL:-}" ] && OLLAMA_MODELS="$OLLAMA_MODELS $OLLAMA_FALLBACK_MODEL"
else
    OLLAMA_MODELS="beam:cloud qwen3.5:9b qwen3.5:4b"
fi
OLLAMA_MODELS="${OLLAMA_MODELS//,/ }"

if [ "$#" -ne 0 ]; then
   INPUT="-- $@"
fi

SELECTED_MODEL=""
primary_model="${OLLAMA_MODELS%% *}"
attempts=0
for model in $OLLAMA_MODELS; do
    [ -n "$model" ] || continue
    if "$SCRIPT_DIR/ollama.sh" run "$model" hi >/dev/null 2>&1; then
        if [ "$attempts" -gt 0 ]; then echo "Primary model unavailable; falling back to $model" >&2; fi
        SELECTED_MODEL="$model"
        break
    fi
    attempts=$((attempts + 1))
done

if [ -z "$SELECTED_MODEL" ]; then
    SELECTED_MODEL="$primary_model"
fi

MODEL="$SELECTED_MODEL"

cd "$SCRIPT_DIR" &&
   exec ./caffeinate.sh ./ollama.sh launch --yes claude --model "$MODEL" $INPUT
