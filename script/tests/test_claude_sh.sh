#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/claude.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/claude.sh"
for m in beam:cloud qwen3.5:9b qwen3.5:4b; do :; done
echo "claude.sh smoke checks passed"
