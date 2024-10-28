#!/bin/bash
set -e

# Store current branch
current_branch=$(git rev-parse --abbrev-ref HEAD)
echo "=== Current branch: $current_branch"

# Get all branches and store in array
mapfile -t branches < <(git for-each-ref --format='%(refname:short)' refs/heads/)

# Process each branch
for branch in "${branches[@]}"; do
    if [ "$branch" = "main" ]; then
        continue
    fi
    
    echo "=== Processing branch: $branch"
    
    # Check if branch needs rebasing
    merge_base=$(git merge-base main "$branch")
    main_head=$(git rev-parse main)
    
    if [ "$merge_base" = "$main_head" ]; then
        echo "=== $branch is already up to date with main"
        continue
    fi
    
    # Attempt rebase
    if ! git checkout "$branch"; then
        echo "=== Failed to checkout $branch"
        exit 1
    fi
    
    if ! git rebase -f main; then
        git rebase --abort
        echo "=== Failed to rebase $branch onto main"
        git checkout "$current_branch"
        exit 1
    fi
    
    echo "=== Successfully rebased $branch onto main"
done

# Return to original branch
git checkout "$current_branch"
echo "=== Rebase operation completed successfully"
