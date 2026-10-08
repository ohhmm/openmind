# Agent Instructions

## One Commit per PR (mandatory)

- Every pull request must contain exactly one commit.
- To change an open PR, amend its commit and force-push (`git commit --amend` then `git push --force-with-lease`); do not add follow-up commits.
- Before pushing, run `git pull --rebase --autostash origin main`.
- Related changes still go in separate PRs, one commit each, based on `main`.

See the full [Git Workflow](docs/knowledge_base/consolidated_knowledge_base.md#git-workflow) in the knowledge base.
