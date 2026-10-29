#!/bin/bash
SCRIPT_DIR=$(dirname "$0")
REQUEST="continue thinking.log on consistent successful mergable resolve of task.md till branch passed CI"

echo $SCRIPT_DIR/update.sh
$SCRIPT_DIR/update.sh &&
	$SCRIPT_DIR/do.sh "$REQUEST" >> thinking.log &&
	git diff &&
	git status &&
	git show
