#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

manifest=$(< "$SCRIPT_DIR/vcpkg.json")
baseline='[{,][[:space:]]*"builtin-baseline"[[:space:]]*:[[:space:]]*"([0-9a-fA-F]{40})"[[:space:]]*[,}]'
if [[ $manifest =~ $baseline ]]; then
    printf '%s\n' "${BASH_REMATCH[1]}"
else
    printf '%s\n' 'builtin-baseline must be a 40-digit Git SHA' >&2
    exit 1
fi
