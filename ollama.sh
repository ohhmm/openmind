#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT="Hi"
OLLAMA_CMD=ollama

if [ "$#" -ne 0 ]; then
    INPUT="$*"
fi

ensure_ollama_installed() {
    for candidate in ollama \
        "/Applications/Ollama.app/Contents//Resources/ollama" \
        "/Applications/Ollama.app/Contents/Resources/ollama" \
        "${LOCALAPPDATA:-}/Programs/Ollama/ollama.exe" \
        "${USERPROFILE:-}/AppData/Local/Programs/Ollama/ollama.exe" \
        "/c/Program Files/Ollama/ollama.exe" \
        "/c/Program Files (x86)/Ollama/ollama.exe"
    do
        if [ -x "$candidate" ] && command -v "$candidate" >/dev/null 2>&1; then
            OLLAMA_CMD="$candidate"
            return 0
        fi
    done

    case "$(uname -s 2>/dev/null || printf 'Unknown')" in
        Linux|Darwin)
            curl -fsSL https://ollama.com/install.sh | sh
            ;;
        MINGW*|MSYS*|CYGWIN*)
            if command -v winget >/dev/null 2>&1; then
                winget install --id Ollama.Ollama --exact --accept-source-agreements --accept-package-agreements
            else
                powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "irm https://ollama.com/install.ps1 | iex"
            fi
            ;;
        *)
            if command -v winget >/dev/null 2>&1; then
                winget install --id Ollama.Ollama --exact --accept-source-agreements --accept-package-agreements
            elif command -v powershell.exe >/dev/null 2>&1; then
                powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "irm https://ollama.com/install.ps1 | iex"
            else
                curl -fsSL https://ollama.com/install.sh | sh
            fi
            ;;
    esac

    hash -r 2>/dev/null || true
    if command -v ollama >/dev/null 2>&1; then
        OLLAMA_CMD=ollama
        return 0
    fi

    for candidate in \
        "/Applications/Ollama.app/Contents//Resources/ollama" \
        "/Applications/Ollama.app/Contents/Resources/ollama" \
        "${LOCALAPPDATA:-}/Programs/Ollama/ollama.exe" \
        "${USERPROFILE:-}/AppData/Local/Programs/Ollama/ollama.exe" \
        "/c/Program Files/Ollama/ollama.exe" \
        "/c/Program Files (x86)/Ollama/ollama.exe"
    do
        if [ -x "$candidate" ] && command -v "$candidate" >/dev/null 2>&1; then
            OLLAMA_CMD="$candidate"
            return 0
        fi
    done

    echo "Failed to install or locate ollama" >&2
    exit 1
}

ensure_ollama_started() {
case "${1:-}" in
    serve|help|-h|--help|-v|--version) ;;
    *)
        case "${OLLAMA_HOST:-127.0.0.1:11434}" in
            127.0.0.1|127.0.0.1:*|localhost|localhost:*|http://127.0.0.1:*|http://localhost:*|'[::1]'|'[::1]':*|'http://[::1]':*)
                if ! "$OLLAMA_CMD" list >/dev/null 2>&1; then
                    server_log="$(mktemp "${TMPDIR:-/tmp}/ollama-server.XXXXXX")" || exit 1
                    nohup "$OLLAMA_CMD" serve >"$server_log" 2>&1 < /dev/null &
                    server_pid=$!
                    attempts=0
                    until "$OLLAMA_CMD" list >/dev/null 2>&1; do
                        if ! kill -0 "$server_pid" 2>/dev/null || [ "$attempts" -ge 30 ]; then
                            cat "$server_log" >&2
                            echo "Ollama startup failed; server log: $server_log" >&2
                            exit 1
                        fi
                        sleep 1
                        attempts=$((attempts + 1))
                    done
                fi
                ;;
        esac
        ;;
esac
}

ensure_ollama() {
    ensure_ollama_installed
    ensure_ollama_started "$@"
}

ensure_ollama "$@"
exec "$OLLAMA_CMD" "$@"
