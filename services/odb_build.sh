#!/usr/bin/env bash
# Minimal ODB code/schema generator using Boost.Optional.
# Also retargets generated includes to <database/*.hxx> so includes resolve
# via the parent include root (…/include).

set -euo pipefail

# --- defaults (override via env) ---
SRC_DIR="${SRC_DIR:-$PWD/include/database}"          # entity .h files
OUT_INCLUDE="${OUT_INCLUDE:-$PWD/include/database}"  # output for .hxx/.ixx
OUT_SRC="${OUT_SRC:-$PWD/src/odb/src}"               # output for .cxx
OUT_SCHEMA="${OUT_SCHEMA:-$PWD/schemas}"             # output for schema.sql
# Colon-separated include roots for ODB's preprocessor
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

log(){ echo "[odb_build] $*"; }
vrun(){ $VERBOSE && { echo "+ $*"; "$@"; } || "$@"; }

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
if (( ${#HEADERS[@]} == 0 )); then
  log "No entity headers in $SRC_DIR"
  exit 0
fi

$VERBOSE && { echo "[odb_build] Headers (${#HEADERS[@]}):"; printf '  - %s\n' "${HEADERS[@]}"; }

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
  log "==> Generating ODB code into temp: $TMP"
  $VERBOSE && set -x
  odb "${ODB_FLAGS[@]}" -o "$TMP" "${HEADERS[@]}"
  $VERBOSE && set +x

  shopt -s nullglob
  GEN_HXX=( "$TMP"/*.hxx )
  GEN_IXX=( "$TMP"/*.ixx )
  GEN_CXX=( "$TMP"/*.cxx )
  shopt -u nullglob

  log "Generated: ${#GEN_HXX[@]} .hxx, ${#GEN_IXX[@]} .ixx, ${#GEN_CXX[@]} .cxx"

  # Move first
  [[ ${#GEN_HXX[@]} -gt 0 ]] && vrun mv -f "${GEN_HXX[@]}" "$OUT_INCLUDE/" || true
  [[ ${#GEN_IXX[@]} -gt 0 ]] && vrun mv -f "${GEN_IXX[@]}" "$OUT_INCLUDE/" || true
  [[ ${#GEN_CXX[@]} -gt 0 ]] && vrun mv -f "${GEN_CXX[@]}" "$OUT_SRC/"     || true

  # Retarget includes inside generated sources/partials to <database/...>
  # We rewrite only quoted includes with basenames ending in .hxx or -odb.hxx.
  changed=0
  shopt -s nullglob
  for f in "$OUT_SRC"/*.cxx "$OUT_INCLUDE"/*.ixx "$OUT_INCLUDE"/*.hxx; do
    [[ -f "$f" ]] || continue
    before="$(md5sum "$f" | awk '{print $1}')"
    vrun perl -0777 -pi -e '
      s{(^\s*#\s*include\s*)"(?:[^"/]*/)?([^"/]+\.hxx)"}
       {$1<database/$2>}mg;
    ' "$f"
    after="$(md5sum "$f" | awk '{print $1}')"
    if [[ "$before" != "$after" ]]; then
      log "retarget: $(basename "$f")"
      ((changed++))
    fi
  done
  shopt -u nullglob
  log "Include retargeting done (${changed} file(s) updated)."
fi

# --- schema ---
if $DO_SCHEMA; then
  log "==> Generating SQL schema"
  SCHEMA_OUT="$OUT_SCHEMA/schema.sql"; rm -f "$SCHEMA_OUT"
  $VERBOSE && set -x
  odb "${SCHEMA_FLAGS[@]}" -o "$TMP" "${HEADERS[@]}"
  $VERBOSE && set +x
  shopt -s nullglob
  if ls "$TMP"/*.sql >/dev/null 2>&1; then
    cat "$TMP"/*.sql > "$SCHEMA_OUT"
    log "==> Schema written to: $SCHEMA_OUT"
  else
    echo "WARN: no .sql emitted."
  fi
  shopt -u nullglob
fi

# --- tip ---
PARENT_INCLUDE="$(cd "$OUT_INCLUDE/.." && pwd)"
log "Done."
log "Tip: add -I\"$PARENT_INCLUDE\" so #include <database/…> resolves."