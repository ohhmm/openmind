#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CODEX_CMD=codex

if ! command -v "$CODEX_CMD" >/dev/null 2>&1; then
    case "$(uname -s 2>/dev/null || printf 'Unknown')" in
        MINGW*|MSYS*|CYGWIN*)
            POWERSHELL_CMD=powershell.exe
            if ! command -v "$POWERSHELL_CMD" >/dev/null 2>&1; then
                POWERSHELL_CMD=pwsh.exe
            fi
            if ! command -v "$POWERSHELL_CMD" >/dev/null 2>&1 || ! command -v cygpath >/dev/null 2>&1; then
                printf 'Windows Codex installation requires PowerShell and cygpath.\n' >&2
                exit 1
            fi
            if [ -n "${CODEX_INSTALL_DIR:-}" ]; then
                CODEX_INSTALL_DIR_WIN="$(cygpath -aw "$CODEX_INSTALL_DIR")" || exit 1
            elif [ -n "${LOCALAPPDATA:-}" ]; then
                CODEX_INSTALL_DIR_WIN="$(cygpath -aw "$LOCALAPPDATA/Programs/OpenAI/Codex/bin")" || exit 1
            else
                printf 'LOCALAPPDATA is not set; cannot determine the Windows Codex install directory.\n' >&2
                exit 1
            fi
            CODEX_CMD="$(cygpath -u "$CODEX_INSTALL_DIR_WIN")/codex.exe" || exit 1
            if [ ! -x "$CODEX_CMD" ]; then
                CODEX_INSTALL_DIR="$CODEX_INSTALL_DIR_WIN" "$POWERSHELL_CMD" -NoProfile -ExecutionPolicy Bypass -Command '$env:CODEX_NON_INTERACTIVE = "1"; Invoke-RestMethod "https://chatgpt.com/codex/install.ps1" | Invoke-Expression'
            fi
            ;;
        *)
            curl -fsSL https://chatgpt.com/codex/install.sh | CODEX_NON_INTERACTIVE=1 sh
            CODEX_CMD="${CODEX_INSTALL_DIR:-$HOME/.local/bin}/codex"
            ;;
    esac
fi

if ! command -v "$CODEX_CMD" >/dev/null 2>&1; then
    printf 'Codex executable not found: %s\n' "$CODEX_CMD" >&2
    exit 1
fi

cd "$SCRIPT_DIR" &&
    exec "$CODEX_CMD" --no-daemon "$@"
