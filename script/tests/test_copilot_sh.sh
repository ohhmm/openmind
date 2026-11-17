#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

bash -n "$ROOT/copilot.sh"
cat > "$TMP_DIR/copilot" <<'EOF'
#!/bin/sh
printf '%s\n' "$PWD" "$@" > "$COPILOT_TEST_LOG"
EOF
chmod +x "$TMP_DIR/copilot"

(
    cd "$TMP_DIR"
    PATH="$TMP_DIR:$PATH" COPILOT_TEST_LOG="$TMP_DIR/log" "$ROOT/copilot.sh" --version "value with spaces"
)
printf '%s\n' "$ROOT" "--version" "value with spaces" > "$TMP_DIR/expected"
cmp "$TMP_DIR/expected" "$TMP_DIR/log"

echo "copilot.sh smoke checks passed"
