#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/do.sh"
bash -n "$ROOT/harness.sh"
bash -n "$ROOT/ollama.sh"
grep -Fq '/Applications/Ollama.app/Contents//Resources/ollama' "$ROOT/ollama.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/harness.sh"
echo "wrapper smoke checks passed"
