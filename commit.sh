#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

REQUEST="cot and push staged files"

"$SCRIPT_DIR/do.sh" "$REQUEST"
git -C "$SCRIPT_DIR" show
