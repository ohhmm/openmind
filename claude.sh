#!/bin/bash


SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# OLLAMA_MODELS is a space- or comma-separated list of models in order of
# preference. Legacy OLLAMA_MODEL / OLLAMA_FALLBACK_MODEL inputs are still
# accepted and folded into the same list.
# OLLAMA_MODELS="${OLLAMA_MODELS:-beam:cloud qwen3.5:9b qwen3.5:4b}"
# OLLAMA_MODELS=beam:cloud qwen3.5:9b qwen3.5:4b
OLLAMA_MODELS="$MODEL $OLLAMA_MODEL $OLLAMA_MODELS $OLLAMA_FALLBACK_MODEL $OLLAMA_FALLBACK_MODELS beam:cloud gemma4:12b qwen3.5:9b qwen3.5:4b"
OLLAMA_MODELS="${OLLAMA_MODELS//,/ }"


if [ "$#" -ne 0 ]; then
   INPUT="-- $@"
fi

if [ -z "$MODEL" ]; then
    if [ -z "$SELECTED_MODEL" ]; then
        for model in $OLLAMA_MODELS; do
            echo "Checking availability of model: $model"
            if "$SCRIPT_DIR/ollama.sh" run "$model" hi ; then
                SELECTED_MODEL="$model"
                break
            else
                echo "Model $model is not available, skipping."
            fi
        done
    fi
    MODEL="$SELECTED_MODEL"
fi

cd "$SCRIPT_DIR" &&
   exec ./caffeinate.sh ./ollama.sh launch --yes claude --model "$MODEL" $INPUT
