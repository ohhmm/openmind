# Agent Instructions

## Terminology
- "ensure X" means make X happen: install, build, fetch, create, and configure it automatically until X holds. It never means only detecting X's absence and raising an error. Positive-only.

## Documentation quallity requirements
- Write only truth, consolidated and correct, in positive-only form
- Love laconism! Save people time of life. Also improves overall productivity across people/agents/etc
- In `if` conditions, order checks sorted by likelihood of happening within its context (more likely first).

## Code quality requirements
- Keep edits minimal and terse
- Prefer preserving existing style, syntax, and formatting over changes that do not alter functionality yet add verbosity.
- preserve existing functionality and settings.
- Do not make changes that reduce code quality, maintainability, readability, testability, test coverage, performance, portability, or safety or any other aspect of quality; if a tradeoff is unavoidable, call it out before proceeding.
- No precision losses, keep precision arbitrary to ensure bugless extensible compatibility
- Avoid empty translation units: when a `.cpp` would otherwise be empty, keep a non-empty anchor symbol in it rather than excluding it from the build, so later additions are not silently dropped.
- Prefer more general, reusable approaches/rules over narrowly specific ones when both are available.
- Prefer automation over manual work when it preserves correctness and quality.
- Avoid `dynamic_cast`; prefer compile-time type checks and `static_cast` or explicit polymorphic interfaces.
- Consider existing helpers for repeated semantic checks; reuse or extend them when doing so improves consistency, readability, and maintainability.
- Represent behavior, capabilities, truthfully, consolidated, correct, in positive-only form and completely in code, tests, examples, comments, documentation, and output.
- Love laconism! Save people time of life. Also improves overall productivity across people/agents/etc
- Disabling tests (e.g. *disabled(), commenting out test cases, or excluding tests from the build) is strictly forbidden. Fix the underlying issue or report it as a blocker instead.

## Refactoring
- Equivalent extraction only: move existing code verbatim; preserve names, order, conditions, paths, and behavior; minimal diff.
- Do not restructure, rename, reorder, reformat, or otherwise "improve" code while extracting; separate improvements into their own PRs.
- Prefer sourcing/importing or calling the extracted unit over changing call sites beyond the moved symbol.

## One Commit per PR (mandatory)

- Every pull request must contain exactly one commit.
- A branch must have exactly one commit on top of `origin/HEAD` (the freshest `origin/main`); after `git fetch`, `git rev-list --count origin/HEAD..HEAD` must be `1`.
- To change an open PR, amend its commit and force-push (`git commit --amend` then `git push --force-with-lease`); do not add follow-up commits.
- Before any pushing, always mandatorily run `git pull --rebase --autostash origin HEAD`
- Related changes still go in separate PRs, one commit each, based on origin/HEAD
- Before creating a PR, check for existing open PRs with the same fix; do not create duplicate PRs. If a fix already exists, amend existing one.

See the full [Git Workflow](docs/knowledge_base/consolidated_knowledge_base.md#git-workflow) in the knowledge base.

## CI feedback loop

- Before pushing, generate the post-push todo/checklist of PR and CI checks to run immediately after push; keep it updated until all required checks pass and failures are resolved.
- Do not stop after opening a PR or seeing its first CI result; inspect checks for the latest PR commit.
- For code or test failures, inspect logs, fix the cause, run relevant local tests when available, amend the existing commit, and push with `--force-with-lease`. Repeat until required CI checks pass.
- Preserve full test output and attribute failures/leaks/crashes to the responsible test; fail unattributed signals instead of discarding them.
- For infrastructure failures, do not change product code or disable tests to hide them; rerun or retry the affected check and report a blocker if it cannot be made to run.
- Obtain full logs and evidence before RCA; do not speculate about causes without observed output.
- Before reporting completion, verify that the latest PR revision has exactly one commit and all required CI checks pass.

## Crossplatform
- Target platforms: Windows, Linux, macOS. Keep changes portable and avoid platform-specific breakage.
