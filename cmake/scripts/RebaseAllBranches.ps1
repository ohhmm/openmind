param(
    [Parameter(Mandatory=$true)]
    [string]$GitExecutable
)

$ErrorActionPreference = 'Stop'
$VerbosePreference = 'Continue'

# Set the Git executable path as an environment variable
$env:GIT_EXECUTABLE = $GitExecutable

Write-Verbose "Getting current branch..."
$current = & $env:GIT_EXECUTABLE rev-parse --abbrev-ref HEAD

Write-Host "All branches before rebase:"
& $env:GIT_EXECUTABLE branch

Write-Verbose "Getting main branch commit hash..."
$mainHash = & $env:GIT_EXECUTABLE rev-parse main
Write-Host "Main branch hash: $mainHash"

Get-Content (& $env:GIT_EXECUTABLE rev-parse --git-dir) + "/config" -ErrorAction SilentlyContinue | Select-String "^\[branch"

foreach ($branch in (& $env:GIT_EXECUTABLE for-each-ref --format='%(refname:short)' refs/heads/)) {
    if ($branch -ne "main") {
        Write-Host "`nProcessing branch: $branch"

        Write-Verbose "Getting branch state before rebase..."
        $beforeHash = & $env:GIT_EXECUTABLE rev-parse $branch
        Write-Host "Before hash: $beforeHash"

        Write-Verbose "Checking out branch $branch..."
        & $env:GIT_EXECUTABLE checkout $branch
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Failed to checkout $branch"
            continue
        }

        Write-Verbose "Getting merge base with main..."
        $mergeBase = & $env:GIT_EXECUTABLE merge-base $branch main
        Write-Host "Merge base with main: $mergeBase"

        Write-Verbose "Attempting to rebase onto main..."
        & $env:GIT_EXECUTABLE rebase -f main
        if ($LASTEXITCODE -ne 0) {
            Write-Verbose "Rebase failed, aborting..."
            & $env:GIT_EXECUTABLE rebase --abort
            Write-Host "Failed to rebase $branch onto main"
            continue
        }

        $afterHash = & $env:GIT_EXECUTABLE rev-parse $branch
        if ($afterHash -eq $beforeHash) {
            Write-Host "Warning: Branch $branch hash unchanged after rebase"
        }
    }
}

Write-Verbose "Returning to original branch..."
& $env:GIT_EXECUTABLE checkout $current

Write-Host "`nAll branches after rebase:"
& $env:GIT_EXECUTABLE branch
