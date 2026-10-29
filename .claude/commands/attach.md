---
description: Attach a file to this conversation: read it and load its contents into context
allowed-tools: Read, Bash(ls:*), AskUserQuestion
argument-hint: <path>
---

## Context

- Requested path: $ARGUMENTS
- Current directory: !`pwd`

## Task

1. Resolve `$ARGUMENTS` as a file path relative to the current directory.
2. If the file is missing, list the parent directory with `ls` and ask the user which existing file they meant.
3. Use `Read` to load the complete file contents into this conversation.
4. Confirm the loaded file (absolute path and line count) and ask what to do with it.

Do not speculate about file contents before reading it. If `$ARGUMENTS` is empty, ask the user for a path.