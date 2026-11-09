#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/do.sh"
bash -n "$ROOT/claude.sh"
bash -n "$ROOT/ollama.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/claude.sh"
echo "fallback model smoke checks passed"
