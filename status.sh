#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
git -C "$SCRIPT_DIR" status --short "$@"
exec git -C "$SCRIPT_DIR" -P log --oneline -s origin/main..HEAD 
