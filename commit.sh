#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

REQUEST="commit and push staged files, use task.md for generating commit message, hiding auth data and tokens mangled"

"$SCRIPT_DIR/do.sh" "$REQUEST"
git -C "$SCRIPT_DIR" show
