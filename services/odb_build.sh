#!/bin/bash

# Usage:
# ./generate_odb_files.sh

set -e

# ----------- CONFIGURATION -----------
SRC_DIR=/home/curious_bytes/Documents/CuriousBee/services/include/database/
OUT_INCLUDE=/home/curious_bytes/Documents/CuriousBee/services/include/odb/
OUT_SRC=/home/curious_bytes/Documents/CuriousBee/services/src/odb/
ODB_INCLUDE_PATH="/home/curious_bytes/Documents/CuriousBee/services/include"

# List files (without extension) to exclude
EXCLUDES=("database" "db_object" "db_types")
# ODB options
ODB_CMD="odb --generate-query --generate-schema --std c++17 -d pgsql -I${ODB_INCLUDE_PATH}"
# -------------------------------------

if [[ -z "$SRC_DIR" || -z "$OUT_INCLUDE" || -z "$OUT_SRC" ]]; then
    echo "Usage: $0 <src_dir> <out_include_dir> <out_src_dir>"
    exit 1
fi

mkdir -p "$OUT_INCLUDE"
mkdir -p "$OUT_SRC"

shopt -s nullglob
for file in "$SRC_DIR"/*.h; do
    filename=$(basename -- "$file" .h)
    # Check if filename is in excludes
    skip=0
    for ex in "${EXCLUDES[@]}"; do
        if [[ "$filename" == "$ex" ]]; then
            skip=1
            break
        fi
    done
    if [[ "$skip" -eq 1 ]]; then
        echo "Skipping excluded: $filename.h"
        continue
    fi

    echo "Generating ODB files for: $filename.h"
    $ODB_CMD -o "$OUT_INCLUDE" --hxx-suffix "-odb.h" "$file"
    $ODB_CMD -o "$OUT_SRC" --cxx-suffix "-odb.cxx" "$file"
done