#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 0 ]; then
   INPUT=$@
else
   INPUT=Hi
fi

cd "$SCRIPT_DIR" &&
   if ! curl -Is --connect-timeout 2 google.com > /dev/null 2>&1; then
  echo "No internet connection detected. Running harness.sh instead."
  time ./harness.sh --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
  exit 0
fi
if [[ -n "${REFLECTION_API_KEY:-}" || -r "${HOME}/.mirror/reflection_api_key" ]]; then
      time  ./reflection-claude-beam.sh --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
   else
      time  ./harness.sh --allow-dangerously-skip-permissions --dangerously-skip-permissions --effort max --no-session-persistence -p "${INPUT}"
   fi
