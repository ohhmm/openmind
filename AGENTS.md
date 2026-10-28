# Agent Instructions

## Code quality requirements
- Keep edits minimal and terse
- preserve existing functionality and settings.
- Do not make changes that reduce code quality, maintainability, readability, testability, test coverage, performance, portability, or safety or any other aspect of quality; if a tradeoff is unavoidable, call it out before proceeding.
- No precision losses
- Prefer more general, reusable approaches/rules over narrowly specific ones when both are available.
- Prefer automation over manual work when it preserves correctness and quality.

## One Commit per PR (mandatory)

- Every pull request must contain exactly one commit.
- A branch must have exactly one commit on top of `origin/HEAD` (the freshest `origin/main`); after `git fetch`, `git rev-list --count origin/HEAD..HEAD` must be `1`.
- To change an open PR, amend its commit and force-push (`git commit --amend` then `git push --force-with-lease`); do not add follow-up commits.
- Before any pushing, always mandatorily run `git pull --rebase --autostash origin HEAD`
- Related changes still go in separate PRs, one commit each, based on origin/HEAD

See the full [Git Workflow](docs/knowledge_base/consolidated_knowledge_base.md#git-workflow) in the knowledge base.

## CI feedback loop

- Do not stop after opening a PR or seeing its first CI result; inspect checks for the latest PR commit.
- For code or test failures, inspect logs, fix the cause, run relevant local tests when available, amend the existing commit, and push with `--force-with-lease`. Repeat until required CI checks pass.
- Preserve full test output and attribute failures/leaks/crashes to the responsible test; fail unattributed signals instead of discarding them.
- For infrastructure failures, do not change product code or disable tests to hide them; rerun or retry the affected check and report a blocker if it cannot be made to run.
- Before reporting completion, verify that the latest PR revision has exactly one commit and all required CI checks pass.

## Crossplatform
- Target platforms: Windows, Linux, macOS. Keep changes portable and avoid platform-specific breakage.
