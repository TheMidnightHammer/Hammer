#!/usr/bin/env bash
# Builds every demo in demo/, moves the built executable to the demo's
# root folder and runs it.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEMO_ROOT="$SCRIPT_DIR/demo"

for demo_dir in "$DEMO_ROOT"/*/; do
    # Skip non-directories
    [ -d "$demo_dir" ] || continue

    demo_name="$(basename "$demo_dir")"
    echo "=========================================="
    echo " Building demo: $demo_name"
    echo "=========================================="

    mkdir -p "$demo_dir/build"
    cd "$demo_dir/build"

    echo "Running cmake for $demo_name..."
    cmake -DCMAKE_BUILD_TYPE=Debug ..

    echo "Running make -j$(nproc) for $demo_name..."
    make -j"$(nproc)"

    # Find the produced executable(s) and move them to the demo's root dir
    shopt -s nullglob
    executables=()
    for file in *; do
        [ -f "$file" ] && [ -x "$file" ] && executables+=("$file")
    done
    shopt -u nullglob

    if [ ${#executables[@]} -eq 0 ]; then
        echo "ERROR: no executable found for $demo_name" >&2
        exit 1
    fi

    for exe in "${executables[@]}"; do
        mv -f "$exe" "$demo_dir"
        echo "Moved '$exe' to $demo_dir"
    done

    cd "$demo_dir"

    # Run the executable from the demo's root dir
    for exe in "${executables[@]}"; do
        echo "Running '$exe'..."
        "./$exe"
        echo "$exe exited successfully"
    done

    cd "$SCRIPT_DIR"
done

echo "All demos built and ran successfully."
