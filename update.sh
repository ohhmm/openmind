#!/bin/bash
SCRIPT_DIR=$(dirname "$0")

cd $SCRIPT_DIR &&
	git pull --rebase --autostash origin main
