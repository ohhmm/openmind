#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/do.sh"
bash -n "$ROOT/harness.sh"
bash -n "$ROOT/ollama.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/harness.sh"
echo "fallback model smoke checks passed"
