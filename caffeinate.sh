#!/bin/bash

if command -v caffeinate >/dev/null 2>&1; then
   exec time caffeinate -i "$@"
else
   exec time $@
fi
