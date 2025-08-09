#!/usr/bin/env bash
# Usage: ./generate_odb_files.sh
set -euo pipefail

# ----------- CONFIG -----------
SRC_DIR="/home/curious_bytes/Documents/CuriousBee/services/include/database"
OUT_INCLUDE="/home/curious_bytes/Documents/CuriousBee/services/include/odb"
OUT_SRC="/home/curious_bytes/Documents/CuriousBee/services/src/odb"
OUT_SCHEMA="/home/curious_bytes/Documents/CuriousBee/services/db/schema"
ODB_INCLUDE_PATH="/home/curious_bytes/Documents/CuriousBee/services/include"

# Exclude basenames (without extension)
EXCLUDES=("database" "db_object" "db_types")

DB_BACKEND="pgsql"
STD="c++17"
TMP_BUILD="$(mktemp -d /tmp/odb_build.XXXXXX)"

# Common flags for code generation
ODB_FLAGS=(
  -d "$DB_BACKEND"
  --std "$STD"
  -I"$ODB_INCLUDE_PATH"
  --generate-query
)

# Schema flags (adjust as needed)
SCHEMA_FLAGS=(
  -d "$DB_BACKEND"
  --std "$STD"
  -I"$ODB_INCLUDE_PATH"
  --generate-schema
  --schema-format sql
  # Uncomment when you start evolving schema:
  # --schema-evolution
  # --schema-version-table schema_version
)

# Suffixes (keep defaults if you want)
HXX_SUFFIX="-odb.hxx"
IXX_SUFFIX="-odb.ixx"
CXX_SUFFIX="-odb.cxx"
# --------------------------------

mkdir -p "$OUT_INCLUDE" "$OUT_SRC" "$OUT_SCHEMA"

# Collect headers, excluding listed basenames
mapfile -d '' HEADERS < <(
  find "$SRC_DIR" -maxdepth 1 -type f -name '*.h' -print0 \
  | while IFS= read -r -d '' f; do
      base="$(basename "${f%.*}")"
      skip=0
      for ex in "${EXCLUDES[@]}"; do
        [[ "$base" == "$ex" ]] && { skip=1; break; }
      done
      (( skip == 0 )) && printf '%s\0' "$f"
    done
)

if (( ${#HEADERS[@]} == 0 )); then
  echo "No headers found to process."
  exit 0
fi

echo "==> Generating ODB code (at-once) into temp dir: $TMP_BUILD"
odb \
  "${SCHEMA_FLAGS[@]}" \
  --input-name all_models \
  --hxx-suffix "$HXX_SUFFIX" \
  --ixx-suffix "$IXX_SUFFIX" \
  --cxx-suffix "$CXX_SUFFIX" \
  --at-once \
  -o "$TMP_BUILD" \
  "${HEADERS[@]}"

echo "==> Moving generated files to include/src"
shopt -s nullglob
# headers
for f in "$TMP_BUILD"/*"$HXX_SUFFIX" "$TMP_BUILD"/*"$IXX_SUFFIX"; do
  [[ -e "$f" ]] && mv -f "$f" "$OUT_INCLUDE/"
done
# sources
for f in "$TMP_BUILD"/*"$CXX_SUFFIX"; do
  [[ -e "$f" ]] && mv -f "$f" "$OUT_SRC/"
done
shopt -u nullglob

echo "==> Generating schema SQL (at-once)"
SCHEMA_OUT="$OUT_SCHEMA/schema.sql"
odb \
  "${SCHEMA_FLAGS[@]}" \
  --input-name all_models \
  --hxx-suffix "$HXX_SUFFIX" \
  --ixx-suffix "$IXX_SUFFIX" \
  --cxx-suffix "$CXX_SUFFIX" \
  --at-once \
  -o "$TMP_BUILD" \
  "${HEADERS[@]}"

# Collect all .sql fragments (ODB may emit per-header or combined, depending on model)
# If a single file is emitted, this still works.
cat "$TMP_BUILD"/*.sql > "$SCHEMA_OUT"

echo "==> Schema written to: $SCHEMA_OUT"
echo "Apply it to your DB manually, e.g.: psql -d yourdb -f \"$SCHEMA_OUT\""

# Cleanup
rm -rf "$TMP_BUILD"
echo "Done."