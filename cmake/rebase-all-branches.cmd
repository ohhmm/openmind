@echo off
setlocal enabledelayedexpansion

for /f "tokens=*" %%b in ('git branch --format="%%%(refname:short)" ^| findstr /v "^main$"') do (
    git checkout %%b
    git rebase main || git rebase --abort
)
git checkout main
