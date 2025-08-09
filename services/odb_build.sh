#!/usr/bin/env bash
# Minimal ODB code/schema generator using Boost.Optional.

set -euo pipefail

# --- defaults (override via env) ---
SRC_DIR="${SRC_DIR:-$PWD/include/database}"          # entity .h files
OUT_INCLUDE="${OUT_INCLUDE:-$PWD/include/database}"  # output for .hxx/.ixx
OUT_SRC="${OUT_SRC:-$PWD/src/odb/src}"               # output for .cxx
OUT_SCHEMA="${OUT_SCHEMA:-$PWD/schemas}"             # output for schema.sql
# Colon-separated include roots for ODB's preprocessor (add more with env)
# Must include: 1) your project include root (contains 'database/'), 2) system include root
ODB_INCLUDE_PATHS="${ODB_INCLUDE_PATHS:-$PWD/include:/usr/include}"
DB_BACKEND="${DB_BACKEND:-pgsql}"
STD="${STD:-c++17}"
INPUT_NAME="${INPUT_NAME:-all_models}"
# Exclude basenames (no extension)
EXCLUDES=(${EXCLUDES:-database db_object db_types models all_models-odb})

VERBOSE=false; DO_CODE=true; DO_SCHEMA=true
while (($#)); do
  case "$1" in
    -v|--verbose) VERBOSE=true;;
    --schema-only) DO_CODE=false; DO_SCHEMA=true;;
    --code-only) DO_CODE=true; DO_SCHEMA=false;;
    -h|--help) echo "Usage: $0 [-v] [--schema-only|--code-only]"; exit 0;;
    *) echo "Unknown arg: $1"; exit 1;;
  esac; shift
done

command -v odb >/dev/null 2>&1 || { echo "ERROR: 'odb' not in PATH"; exit 1; }
mkdir -p "$OUT_INCLUDE" "$OUT_SRC" "$OUT_SCHEMA"
TMP="$(mktemp -d /tmp/odb_build.XXXXXX)"; trap 'rm -rf "$TMP"' EXIT

# --- collect entity headers (flat dir, exclude umbrellas/generated) ---
mapfile -d '' HEADERS < <(
  find "$SRC_DIR" -maxdepth 1 -type f -name '*.h' -print0 |
  while IFS= read -r -d '' f; do
    b="${f##*/}"; b="${b%.*}"
    for ex in "${EXCLUDES[@]}"; do [[ "$b" == "$ex" ]] && continue 2; done
    [[ "$b" == *-odb* || "$b" == all_models ]] && continue
    printf '%s\0' "$f"
  done | sort -z
)
(( ${#HEADERS[@]} )) || { echo "No entity headers in $SRC_DIR"; exit 0; }

$VERBOSE && { echo "Headers (${#HEADERS[@]}):"; printf '  - %s\n' "${HEADERS[@]}"; }

# --- build include flags from ODB_INCLUDE_PATHS (colon-separated) ---
COMMON_I=()
IFS=':' read -r -a _incs <<< "$ODB_INCLUDE_PATHS"
for inc in "${_incs[@]}"; do [[ -n "$inc" ]] && COMMON_I+=( -I"$inc" ); done

# --- flags ---
HIX=( --hxx-suffix .hxx --ixx-suffix .ixx --cxx-suffix .cxx )
MODE=( --at-once --input-name "$INPUT_NAME" )
WRAP=$'#include <boost/optional.hpp>\n#include <odb/boost/optional/wrapper-traits.hxx>\n'

ODB_FLAGS=(
  -d "$DB_BACKEND" --std "$STD" "${COMMON_I[@]}"
  --generate-query "${MODE[@]}" "${HIX[@]}"
  --odb-prologue "$WRAP"
  --hxx-prologue "$WRAP"
  --default-pointer std::shared_ptr
)

SCHEMA_FLAGS=(
  -d "$DB_BACKEND" --std "$STD" "${COMMON_I[@]}"
  --generate-schema --schema-format sql "${MODE[@]}" "${HIX[@]}"
  --odb-prologue "$WRAP"
  --hxx-prologue "$WRAP"
  --default-pointer std::shared_ptr
)

# --- codegen ---
if $DO_CODE; then
  echo "==> Generating ODB code into: $TMP"
  $VERBOSE && set -x
  odb "${ODB_FLAGS[@]}" -o "$TMP" "${HEADERS[@]}"
  $VERBOSE && set +x
  shopt -s nullglob
  mv -f "$TMP"/*.hxx "$TMP"/*.ixx "$OUT_INCLUDE/" 2>/dev/null || true
  mv -f "$TMP"/*.cxx "$OUT_SRC/" 2>/dev/null || true
  shopt -u nullglob
fi

# --- schema ---
if $DO_SCHEMA; then
  echo "==> Generating SQL schema"
  SCHEMA_OUT="$OUT_SCHEMA/schema.sql"; rm -f "$SCHEMA_OUT"
  $VERBOSE && set -x
  odb "${SCHEMA_FLAGS[@]}" -o "$TMP" "${HEADERS[@]}"
  $VERBOSE && set +x
  shopt -s nullglob
  cat "$TMP"/*.sql > "$SCHEMA_OUT" 2>/dev/null || echo "WARN: no .sql emitted."
  shopt -u nullglob
  [[ -f "$SCHEMA_OUT" ]] && echo "==> Schema written to: $SCHEMA_OUT"
fi

echo "Done."