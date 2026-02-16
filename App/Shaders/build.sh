#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_DIR="$SCRIPT_DIR/bin"
FRAG_S="Fragment.frag"
VERT_S="Vertex.vert"

glslc $SCRIPT_DIR/$FRAG_S -o $BUILD_DIR/$FRAG_S.spv
glslc $SCRIPT_DIR/$VERT_S -o $BUILD_DIR/$VERT_S.spv