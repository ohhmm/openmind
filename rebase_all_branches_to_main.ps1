# rebase_all_branches_to_main.ps1

# Store the current branch name
$currentBranch = git rev-parse --abbrev-ref HEAD

# Fetch the latest changes from the remote
git fetch --all

# Get all local branches
$branches = git for-each-ref --format='%(refname:short)' refs/heads/

foreach ($branch in $branches) {
    Write-Host "Processing branch: $branch"

    # Checkout the branch
    git checkout $branch

    # Attempt to rebase onto main
    $rebaseResult = git rebase origin/main 2>&1

    # Check if rebase was successful
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Successfully rebased $branch onto main"
    }
    else {
        Write-Host "Failed to rebase $branch onto main. Error: $rebaseResult"
        Write-Host "Aborting rebase for this branch"
        git rebase --abort
    }
}

# Return to the original branch
git checkout $currentBranch

Write-Host "Rebase process completed. Returned to branch: $currentBranch"
