#!/usr/bin/env bash
# Deletes all build artifacts from every demo in demo/:
#   - the build/ directory
#   - any compiled executable (ELF binary) left in the demo's root folder
#     (e.g. voxel_Engine)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEMO_ROOT="$SCRIPT_DIR/demo"

for demo_dir in "$DEMO_ROOT"/*/; do
    # Skip non-directories
    [ -d "$demo_dir" ] || continue

    demo_name="$(basename "$demo_dir")"
    echo "=========================================="
    echo " Cleaning demo: $demo_name"
    echo "=========================================="

    # Remove the build directory
    if [ -d "$demo_dir/build" ]; then
        rm -rf "$demo_dir/build"
        echo "Removed build/"
    else
        echo "No build/ directory found"
    fi

    # Remove imgui.ini left over by the engine
    if [ -f "$demo_dir/imgui.ini" ]; then
        echo "Removing imgui.ini"
        rm -f "$demo_dir/imgui.ini"
    fi

    # Remove any compiled binaries left in the demo's root folder.
    # Only ELF executables are deleted, so scripts like compile_Shaders
    # are left untouched.
    for file in "$demo_dir"*; do
        [ -f "$file" ] && [ -x "$file" ] || continue
        if [ "$(head -c 4 "$file" | od -An -tx1 | tr -d ' \n')" = "7f454c46" ]; then
            echo "Removing binary: $(basename "$file")"
            rm -f "$file"
        fi
    done
done

echo "All build artifacts removed."
