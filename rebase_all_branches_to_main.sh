#!/bin/bash
set -e
set -x

# Get git executable and repo directory
GIT_EXECUTABLE=$(which git)
REPO_DIR="${1:-.}"
cd "$REPO_DIR"

# Store current branch to return to it later
CURRENT_BRANCH=$("$GIT_EXECUTABLE" rev-parse --abbrev-ref HEAD)
echo "Current branch is $CURRENT_BRANCH"

# First ensure we're up to date with remote
echo "Fetching updates from remote..."
"$GIT_EXECUTABLE" fetch --all

# Make sure main branch exists
if ! "$GIT_EXECUTABLE" show-ref --verify --quiet refs/heads/main; then
    echo "Error: main branch does not exist"
    exit 1
fi

# Switch to main and update it
echo "Updating main branch..."
"$GIT_EXECUTABLE" checkout main || { echo "Failed to checkout main branch"; exit 1; }
"$GIT_EXECUTABLE" pull --ff-only || { echo "Failed to update main branch"; exit 1; }

# Get main branch commit hash for explicit rebasing
MAIN_COMMIT=$("$GIT_EXECUTABLE" rev-parse main)
echo "Main branch commit: $MAIN_COMMIT"

# Get all local branches
echo "Getting list of local branches..."
BRANCHES=()
while IFS= read -r branch; do
    BRANCHES+=("$branch")
done < <("$GIT_EXECUTABLE" for-each-ref --format='%(refname:short)' refs/heads/)

# Rebase each branch onto main
for branch in "${BRANCHES[@]}"; do
    if [ "$branch" != "main" ]; then
        echo "Processing branch: $branch"
        if "$GIT_EXECUTABLE" checkout "$branch"; then
            echo "Synchronizing branch with main..."
            if ! "$GIT_EXECUTABLE" pull --rebase --autostash origin main; then
                echo "Failed to synchronize branch $branch with main"
                continue
            fi
            # Store the branch's original HEAD
            BRANCH_HEAD=$("$GIT_EXECUTABLE" rev-parse HEAD)

            # Find the merge base between this branch and main
            MERGE_BASE=$("$GIT_EXECUTABLE" merge-base "$branch" main)
            echo "Found merge base ${MERGE_BASE} between $branch and main"

            # Get list of commits unique to this branch
            UNIQUE_COMMITS=()
            while IFS= read -r commit; do
                UNIQUE_COMMITS+=("$commit")
            done < <("$GIT_EXECUTABLE" rev-list --reverse "${MERGE_BASE}".."${branch}")

            echo "Found ${#UNIQUE_COMMITS[@]} unique commits in $branch"

            # Reset branch to main
            if "$GIT_EXECUTABLE" reset --hard main; then
                # Cherry-pick each commit
                FAILED=0
                for commit in "${UNIQUE_COMMITS[@]}"; do
                    if ! "$GIT_EXECUTABLE" cherry-pick "$commit"; then
                        echo "Conflict detected while cherry-picking $commit"
                        "$GIT_EXECUTABLE" cherry-pick --abort
                        # Reset back to original state
                        "$GIT_EXECUTABLE" reset --hard "${BRANCH_HEAD}"
                        FAILED=1
                        break
                    fi
                done

                if [ $FAILED -eq 0 ]; then
                    NEW_HEAD=$("$GIT_EXECUTABLE" rev-parse HEAD)
                    echo "Successfully rebased $branch onto main"
                    echo "New HEAD: ${NEW_HEAD}"
                    if [ "${BRANCH_HEAD}" = "${NEW_HEAD}" ]; then
                        echo "Warning: Branch HEAD unchanged after rebase"
                    fi
                else
                    echo "Branch $branch remains at original commit ${BRANCH_HEAD}"
                fi
            else
                echo "Failed to reset branch $branch to main"
            fi
        else
            echo "Failed to checkout branch $branch"
        fi
    fi
done

# Return to original branch
echo "Returning to original branch $CURRENT_BRANCH..."
"$GIT_EXECUTABLE" checkout "$CURRENT_BRANCH"
