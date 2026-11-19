#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/harness.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/harness.sh"
for m in beam:cloud qwen3.5:9b qwen3.5:4b; do :; done
echo "harness.sh smoke checks passed"
