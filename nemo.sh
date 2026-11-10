#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT=--
MODEL="${OLLAMA_MODEL:-nvidia/nemotron-3.5-super-vl}"
CLAUDE_CODE_MAX_CONTEXT_TOKENS=1048576
ANTHROPIC_DEFAULT_OPUS_MODEL=$MODEL
API_TIMEOUT_MS=1800000
ANTHROPIC_BASE_URL=http://localhost:1234
ANTHROPIC_AUTH_TOKEN=lmstudio


if [ "$#" -ne 0 ]; then
   INPUT="-- $@"
fi

cd "$SCRIPT_DIR" &&
   exec ./caffeinate.sh  claude --model "$MODEL" $INPUT
