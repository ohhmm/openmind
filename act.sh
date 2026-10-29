#!/bin/bash

DIR="${1:-.}"
REQUEST="continue thinking.log on consistent successful mergable resolve of task.md till branch passed CI"

./update.sh &&
	$DIR/do.sh "$REQUEST" >> thinking.log &&
	git diff &&
	git status &&
	git show

