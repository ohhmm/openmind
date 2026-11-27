#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || exit 1

run_pull() {
    LC_ALL=C git pull --rebase --autostash origin main 2>&1
}

output="$(run_pull)"
status=$?

if (( status == 0 )); then
    if [[ -n $output ]]; then
        printf '%s\n' "$output"
    fi
    exit 0
fi

files="$(printf '%s\n' "$output" | sed -n '/would be overwritten by checkout:/,/Please move or remove/p' | sed -n 's/^  //p')"

if [[ -z $files ]]; then
    printf '%s\n' "$output" >&2
    exit "$status"
fi

tmpdir="$(mktemp -d)" || exit 1

moved=()
while IFS= read -r f; do
    [[ -z $f ]] && continue
    if [[ -e $f ]]; then
        mkdir -p "$tmpdir/$(dirname "$f")"
        mv -- "$f" "$tmpdir/$f"
        moved+=("$f")
    fi
done <<< "$files"

output2="$(run_pull)"
status2=$?

for f in "${moved[@]}"; do
    mkdir -p "$(dirname "$f")"
    mv -f -- "$tmpdir/$f" "$f"
done

rm -rf "$tmpdir"

if (( status2 == 0 )); then
    if [[ -n $output2 ]]; then
        printf '%s\n' "$output2"
    fi
    exit 0
fi

printf '%s\n' "$output2" >&2
exit "$status2"
