#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 0 ]; then
   INPUT=$@
else
   INPUT=Hi
fi

cd "$SCRIPT_DIR" &&
   if [[ -n "${REFLECTION_API_KEY:-}" || -r "${HOME}/.mirror/reflection_api_key" ]]; then
      exec ./reflection-claude-beam.sh --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
   else
      exec ./harness.sh --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
   fi
