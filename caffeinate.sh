#!/bin/bash

if command -v caffeinate >/dev/null 2>&1; then
   time caffeinate -i "$@"
else
   time $@
fi
