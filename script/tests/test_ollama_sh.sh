#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/ollama.sh"
grep -Fq '/Applications/Ollama.app/Contents//Resources/ollama' "$ROOT/ollama.sh"
grep -Fq '/Applications/Ollama.app/Contents/Resources/ollama' "$ROOT/ollama.sh"
grep -Fq 'OLLAMA_MODELS' "$ROOT/ollama.sh"
echo "ollama.sh smoke checks passed"
