#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/do.sh"
bash -n "$ROOT/claude.sh"
bash -n "$ROOT/ollama.sh"
grep -Fq '/Applications/Ollama.app/Contents//Resources/ollama' "$ROOT/ollama.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/claude.sh"
echo "wrapper smoke checks passed"
