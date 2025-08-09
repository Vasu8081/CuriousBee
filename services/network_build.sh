#!/usr/bin/env bash
# Compile Cap'n Proto schema(s) via a temp dir, then place:
#   *.capnp.c++  -> OUT_SRC
#   *.capnp.h    -> OUT_INCLUDE
#
# Usage:
#   ./capnp_build.sh [--src DIR] [--schema FILE] [--out-src DIR] [--out-include DIR] [-v] [--clean]

set -euo pipefail

SRC_DIR="$PWD/"
SCHEMA=""
OUT_SRC="$PWD/src/messages/src"
OUT_INCLUDE="$PWD/src/include/messages"
VERBOSE=false
CLEAN=false

log() {
    echo "[capnp_build] $1"
}

while (($#)); do
  case "$1" in
    --src)           SRC_DIR="$2"; shift 2 ;;
    --schema)        SCHEMA="$2"; shift 2 ;;
    --out-src)       OUT_SRC="$2"; shift 2 ;;
    --out-include)   OUT_INCLUDE="$2"; shift 2 ;;
    -v|--verbose)    VERBOSE=true; shift ;;
    --clean)         CLEAN=true; shift ;;
    -h|--help)
      cat <<EOF
Usage: $0 [--src DIR] [--schema FILE] [--out-src DIR] [--out-include DIR] [-v] [--clean]
EOF
      exit 0 ;;
    *) echo "Unknown arg: $1" >&2; exit 1 ;;
  esac
done

log "Source dir: $SRC_DIR"
log "Output src dir: $OUT_SRC"
log "Output include dir: $OUT_INCLUDE"

# Resolve default schema
if [[ -z "$SCHEMA" ]]; then
  SCHEMA="${SRC_DIR%/}/schemas/network_msg.capnp"
fi
log "Schema file: $SCHEMA"

# Check tool
if ! command -v capnp >/dev/null 2>&1; then
  log "ERROR: 'capnp' compiler not found in PATH."
  exit 1
fi
log "Found capnp compiler."

# Check schema exists
if [[ ! -f "$SCHEMA" ]]; then
  log "Schema not found, skipping build."
  exit 0
fi

# Prepare output dirs
log "Creating output directories..."
mkdir -p "$OUT_SRC" "$OUT_INCLUDE"
log "Output directories ready."

# Clean old files if requested
if $CLEAN; then
  log "Cleaning previous generated files..."
  find "$OUT_SRC" -maxdepth 1 -type f -name '*.capnp.c++' -delete || true
  find "$OUT_INCLUDE" -maxdepth 1 -type f -name '*.capnp.h' -delete || true
  log "Clean completed."
fi

# Temp build dir
TMPDIR="$(mktemp -d /tmp/capnp_build.XXXXXX)"
trap 'rm -rf "$TMPDIR"' EXIT
log "Temporary directory created: $TMPDIR"

# Compile schema
log "Compiling schema..."
$VERBOSE && set -x
capnp compile \
  -oc++:"$TMPDIR" \
  --src-prefix="$SRC_DIR" \
  -I "$SRC_DIR" \
  "$SCHEMA"
$VERBOSE && set +x
log "Compilation completed."

# Move generated files
log "Moving generated .capnp.c++ files to $OUT_SRC..."
moved_any=false
shopt -s nullglob
for f in "$TMPDIR"/*.capnp.c++; do
  mv -f "$f" "$OUT_SRC/"
  log "Moved source: $(basename "$f")"
  moved_any=true
done

log "Moving generated .capnp.h files to $OUT_INCLUDE..."
for f in "$TMPDIR"/*.capnp.h; do
  mv -f "$f" "$OUT_INCLUDE/"
  log "Moved header: $(basename "$f")"
  moved_any=true
done
shopt -u nullglob

if ! $moved_any; then
  log "WARNING: No generated files found. Check if schema produced outputs."
else
  log "All files moved successfully."
fi

log "Cap'n Proto build done."