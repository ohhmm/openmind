#!/bin/bash


SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CMAKE_CMD="cmake"
CMAKE_VERSION="${CMAKE_VERSION:-4.4.4}"

is_executable() {
    [ -n "${1:-}" ] || return 1
    if [[ "$1" == */* ]]; then
        [ -f "$1" ] && [ -x "$1" ]
    else
        command -v "$1" >/dev/null 2>&1
    fi
}

ensure_cmake() {
    for candidate in \
        "cmake" \
        "cmake3" \
        "/Applications/CMake.app/Contents/bin/cmake" \
        "/Applications/CMake.app/Contents/MacOS/CMake" \
        "/usr/local/bin/cmake" \
        "/opt/homebrew/bin/cmake" \
        "/usr/bin/cmake" \
        "/snap/bin/cmake" \
        "${HOME:-}/.local/bin/cmake" \
        "${HOME:-}/.local/cmake/bin/cmake" \
        "${LOCALAPPDATA:-}/Programs/CMake/bin/cmake.exe" \
        "${ProgramFiles:-}/CMake/bin/cmake.exe" \
        "C:/Program Files/CMake/bin/cmake.exe" \
        "C:/Program Files (x86)/CMake/bin/cmake.exe"
    do
        if is_executable "$candidate"; then
            CMAKE_CMD="$candidate"
            return 0
        fi
    done

    case "$(uname -s 2>/dev/null || printf 'Unknown')" in
        Darwin)
            if command -v brew >/dev/null 2>&1; then
                brew install cmake
            elif command -v port >/dev/null 2>&1; then
                sudo port install cmake
            else
                install_cmake_from_archive "macos-universal" "tar.gz" "/Applications" "CMake.app"
            fi
            ;;
        Linux)
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update
                sudo apt-get install -y cmake
            elif command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y cmake
            elif command -v yum >/dev/null 2>&1; then
                sudo yum install -y cmake
            elif command -v pacman >/dev/null 2>&1; then
                sudo pacman -Sy --noconfirm cmake
            elif command -v zypper >/dev/null 2>&1; then
                sudo zypper install -y cmake
            elif command -v apk >/dev/null 2>&1; then
                sudo apk add --no-cache cmake
            elif command -v brew >/dev/null 2>&1; then
                brew install cmake
            elif command -v port >/dev/null 2>&1; then
                sudo port install cmake
            else
                install_cmake_from_archive "$(cmake_linux_arch)" "tar.gz" "${HOME}/.local" "cmake"
            fi
            ;;
        MINGW*|MSYS*|CYGWIN*)
            if command -v winget >/dev/null 2>&1; then
                winget install --id Kitware.CMake --exact --accept-source-agreements --accept-package-agreements
            elif command -v choco >/dev/null 2>&1; then
                choco install cmake -y
            elif command -v scoop >/dev/null 2>&1; then
                scoop install cmake
            else
                install_cmake_from_archive "windows-x86_64" "zip" "${LOCALAPPDATA:-$HOME}/Programs" "cmake"
            fi
            ;;
        *)
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update && sudo apt-get install -y cmake
            elif command -v brew >/dev/null 2>&1; then
                brew install cmake
            elif command -v winget >/dev/null 2>&1; then
                winget install --id Kitware.CMake --exact --accept-source-agreements --accept-package-agreements
            else
                install_cmake_from_archive "linux-x86_64" "tar.gz" "${HOME}/.local" "cmake"
            fi
            ;;
    esac

    hash -r 2>/dev/null || true

    for candidate in \
        "cmake" "cmake3" \
        "/Applications/CMake.app/Contents/bin/cmake" \
        "/usr/local/bin/cmake" "/opt/homebrew/bin/cmake" "/usr/bin/cmake" \
        "/snap/bin/cmake" "${HOME:-}/.local/bin/cmake" \
        "${HOME:-}/.local/cmake/bin/cmake" \
        "${LOCALAPPDATA:-}/Programs/CMake/bin/cmake.exe" \
        "${ProgramFiles:-}/CMake/bin/cmake.exe" \
        "C:/Program Files/CMake/bin/cmake.exe"
    do
        if is_executable "$candidate"; then
            CMAKE_CMD="$candidate"
            return 0
        fi
    done

    echo "Failed to install or locate cmake" >&2
    exit 1
}

cmake_linux_arch() {
    case "$(uname -m 2>/dev/null || printf x86_64)" in
        x86_64|amd64) printf 'linux-x86_64' ;;
        aarch64|arm64) printf 'linux-aarch64' ;;
        *) printf 'linux-x86_64' ;;
    esac
}

install_cmake_from_archive() {
    # $1: archive slug, $2: extension, $3: install parent, $4: expected entry hint
    local slug="$1" extension="$2" parent="$3" hint="$4"
    local url archive tmp extracted app binary
    url="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-${slug}.${extension}"
    archive="$(mktemp).${extension}"
    tmp="$(mktemp -d)"
    curl -fsSL "$url" -o "$archive"
    case "$extension" in
        tar.gz|tgz) tar -xzf "$archive" -C "$tmp" ;;
        zip) unzip -q "$archive" -d "$tmp" ;;
        *) echo "Unsupported archive type: $extension" >&2; return 1 ;;
    esac

    mkdir -p "$parent"
    app="$(find "$tmp" -type d -name 'CMake.app' | head -n 1)"
    if [ -n "$app" ]; then
        rm -rf "$parent/CMake.app"
        mv "$app" "$parent/CMake.app"
        return 0
    fi

    binary="$(find "$tmp" -type f -name 'cmake' -o -type f -name 'cmake.exe' | head -n 1)"
    if [ -n "$binary" ]; then
        extracted="$(dirname "$(dirname "$binary")")"
        local name
        name="$(basename "$extracted")"
        rm -rf "$parent/$name"
        mv "$extracted" "$parent/$name"
        if [ -x "$parent/$name/bin/cmake" ]; then
            ln -sf "$parent/$name/bin/cmake" "${HOME}/.local/bin/cmake" 2>/dev/null || true
        fi
        return 0
    fi

    echo "Downloaded CMake archive but could not locate executable" >&2
    return 1
}

ensure_cmake
exec "$CMAKE_CMD" "$@"
