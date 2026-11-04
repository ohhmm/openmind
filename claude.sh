#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT=--
MODEL="${OLLAMA_MODEL:-beam:cloud}"
FALLBACK_MODEL="${OLLAMA_FALLBACK_MODEL:-qwen3.5:9b}"

if [ "$#" -ne 0 ]; then
   INPUT="-- $@"
fi

cd "$SCRIPT_DIR" &&
   exec ./caffeinate.sh ./ollama.sh launch --yes claude --model "$MODEL" $INPUT
