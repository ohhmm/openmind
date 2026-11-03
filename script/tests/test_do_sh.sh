#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
bash -n "$ROOT/do.sh"
grep -Fq '/Applications/Ollama.app/Contents//Resources/ollama' "$ROOT/do.sh"
echo "do.sh smoke checks passed"
