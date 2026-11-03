#!/bin/bash

if command -v caffeinate >/dev/null 2>&1; then
    exec caffeinate -i "$@"
fi
exec "$@"
