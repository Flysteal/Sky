#!/usr/bin/env bash
set -euo pipefail

if [[ ! -x "$0" ]]; then
    echo "Error: $(basename "$0") is not executable."
    echo "Fix with: chmod +x $(basename "$0")"
    exit 1
fi

RUN_AFTER_BUILD=false
COMPILE_SHADERS=false
for arg in "$@"; do
    if [[ "$arg" == "-x" ]]; then
        RUN_AFTER_BUILD=true
        break
    fi
    if [[ "$arg" == "-s" ]]; then
        COMPILE_SHADERS=true
        break
    fi
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cmake -S "$SCRIPT_DIR" -B "$SCRIPT_DIR/build" -G Ninja
cmake --build "$SCRIPT_DIR/build"

EXECUTABLE="$SCRIPT_DIR/build/SkyLands"

if $RUN_AFTER_BUILD; then
    kitty -e bash -c "\"$EXECUTABLE\"; read"
fi

if $COMPILE_SHADERS; then
    $SCRIPT_DIR/App/Shaders/build.sh
fi
