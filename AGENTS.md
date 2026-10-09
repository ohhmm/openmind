# Agent Instructions

## One Commit per PR (mandatory)

- Every pull request must contain exactly one commit.
- To change an open PR, amend its commit and force-push (`git commit --amend` then `git push --force-with-lease`); do not add follow-up commits.
- Before pushing, run `git pull --rebase --autostash origin main`.
- Related changes still go in separate PRs, one commit each, based on `main`.

See the full [Git Workflow](docs/knowledge_base/consolidated_knowledge_base.md#git-workflow) in the knowledge base.

## Crossplatform
- Target platforms: Windows, Linux, macOS. Keep changes portable and avoid platform-specific breakage.

## Code quality requirements
- Keep edits minimal and terse
- preserve existing functionality and settings.
- Do not make changes that reduce code quality, maintainability, readability, testability, test coverage, performance, portability, or safety or any other aspect of quality; if a tradeoff is unavoidable, call it out before proceeding.
- No precision losses
