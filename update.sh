#!/bin/bash

DIR="${1:-.}"

cd $DIR &&
	git pull --rebase --autostash origin main
