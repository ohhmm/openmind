---
description: Pre-push quality gate: collect QA bullets from AGENTS.md, CLAUDE.md, and referenced docs; model-assess each bullet; fix/reiterate until clean; then push
allowed-tools: Bash(python3:*), Bash(git:*), Read, AskUserQuestion
---

## QA checklist collection

Run the collector before assessment:

!`python3 "${CLAUDE_PROJECT_DIR}/.claude/scripts/pre_push_qa_collect.py"`

Then use `Read` to load `.claude/pre-push-checklist.md`.

## Task

1. If the checklist has no bullets, stop and tell the user that no QA sources were found. Ask whether to create `AGENTS.md` or `CLAUDE.md` first.

2. Assess **every bullet** in `.claude/pre-push-checklist.md`, including bullets from recursively referenced Markdown files. For each bullet, use the current/decision model to determine a strict boolean:
   - `true`: the repository currently satisfies the bullet.
   - `false`: it does not.

   For each item, inspect the repository, git state, tests, CI configuration, documentation, and relevant files before answering. Do not guess.

3. Present a table with these columns:
   - Source
   - Bullet
   - Compliant (`true` or `false`)
   - Evidence

4. If every row is `true`:
   - Exit pre-push sequence successfully.

5. If any row is `false`:
   - Show a second table containing only non-compliant bullets, with evidence and the smallest useful remediation.
   - Ask the user whether to address the failures now.
   - If yes, make the changes, run the relevant checks, then re-run this entire command flow until there are no failures.
   - If no, stop and do not push.

6. Report final state:
   - Total QA bullets
   - Number compliant
   - Number non-compliant
   - Whether policy history check passed
   - Whether push allowed
