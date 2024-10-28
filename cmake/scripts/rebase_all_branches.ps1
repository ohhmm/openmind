param(
    [string]$GitExecutable = "git",
    [string]$MainBranch = "main"
)

function Write-Log {
    param([string]$Message)
    Write-Host "===$Message"
}

try {
    # Store current branch
    $currentBranch = & $GitExecutable rev-parse --abbrev-ref HEAD
    Write-Log "Current branch: $currentBranch"

    # Get all branches
    $branches = & $GitExecutable branch | ForEach-Object { $_.TrimStart('* ') }
    
    foreach ($branch in $branches) {
        if ($branch -eq $MainBranch) { continue }
        
        Write-Log "Processing branch: $branch"
        try {
            # Check if branch needs rebasing
            $mergeBase = & $GitExecutable merge-base $MainBranch $branch
            $mainHead = & $GitExecutable rev-parse $MainBranch
            
            if ($mergeBase -eq $mainHead) {
                Write-Log "$branch is already up to date with $MainBranch"
                continue
            }

            # Attempt rebase
            & $GitExecutable checkout $branch
            if ($LASTEXITCODE -ne 0) { throw "Failed to checkout $branch" }
            
            & $GitExecutable rebase -f $MainBranch
            if ($LASTEXITCODE -ne 0) { 
                & $GitExecutable rebase --abort
                throw "Failed to rebase $branch onto $MainBranch"
            }
            
            Write-Log "Successfully rebased $branch onto $MainBranch"
        }
        catch {
            Write-Error "Error processing branch $branch`: $_"
            exit 1
        }
    }

    # Return to original branch
    & $GitExecutable checkout $currentBranch
    Write-Log "Rebase operation completed successfully"
    exit 0
}
catch {
    Write-Error "Fatal error: $_"
    exit 1
}
