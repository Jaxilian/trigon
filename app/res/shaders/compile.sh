#!/bin/bash

set -e

SOURCE_DIR="./"
TARGET_DIR="../../bin/debug/data/shaders"

# Create target directory if it doesn't exist
mkdir -p "$TARGET_DIR"

# Iterate over all .vert and .frag files in the source directory
for f in "$SOURCE_DIR"/*.vert "$SOURCE_DIR"/*.frag; do
    if [[ "$f" == *.vert ]]; then
        OUT_FILE=$(basename "$f" .vert).vert.spv
    elif [[ "$f" == *.frag ]]; then
        OUT_FILE=$(basename "$f" .frag).frag.spv
    else
        echo "Unknown file type: $f"
        continue
    fi

    # Compile the shader
    "$VULKAN_SDK/Bin/glslc" "$f" -o "$TARGET_DIR/$OUT_FILE"
done
