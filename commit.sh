#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

REQUEST="commit and push only staged files, use task.md for generating commit message if it relevant, hiding auth data and tokens mangled"

"$SCRIPT_DIR/do.sh" "$REQUEST" &&
    git -C "$SCRIPT_DIR" show
