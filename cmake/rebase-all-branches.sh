#!/bin/bash
git fetch --all
for branch in $(git branch --format='%(refname:short)' | grep -v '^main$'); do
    git checkout "$branch" && git rebase main
done
git checkout main
