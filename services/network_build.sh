#!/usr/bin/env bash
# Compile Cap'n Proto schema(s) via a temp dir, then place:
#   *.capnp.c++  -> OUT_SRC
#   *.capnp.h    -> OUT_INCLUDE
# And rewrite generated .c++ includes from "X.capnp.h" -> <messages/X.capnp.h>
#
# Usage:
#   ./capnp_build.sh [--src DIR] [--schema FILE] [--out-src DIR] [--out-include DIR] [-v] [--clean]
#
# Defaults:
#   --src          $PWD/
#   --schema       <src>/schemas/network_msg.capnp
#   --out-src      $PWD/src/messages/src
#   --out-include  $PWD/include/messages

set -euo pipefail

# ---------- defaults ----------
SRC_DIR="$PWD/"
SCHEMA=""  # resolved below
OUT_SRC="$PWD/src/messages/src"
OUT_INCLUDE="$PWD/include/messages"
VERBOSE=false
CLEAN=false

log() { echo "[capnp_build] $*"; }
vrun() { $VERBOSE && { echo "+ $*"; "$@"; } || "$@"; }

# ---------- args ----------
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
Defaults:
  --src          $SRC_DIR
  --schema       <src>/schemas/network_msg.capnp
  --out-src      $OUT_SRC
  --out-include  $OUT_INCLUDE
EOF
      exit 0 ;;
    *)
      echo "Unknown arg: $1" >&2
      exit 1 ;;
  esac
done

# ---------- resolve schema default ----------
if [[ -z "$SCHEMA" ]]; then
  SCHEMA="${SRC_DIR%/}/schemas/network_msg.capnp"
fi

# ---------- banner ----------
log "Source dir:      $SRC_DIR"
log "Output src dir:  $OUT_SRC"
log "Output include:  $OUT_INCLUDE"
log "Schema file:     $SCHEMA"

# ---------- tool checks ----------
if ! command -v capnp >/dev/null 2>&1; then
  echo "ERROR: 'capnp' compiler not found in PATH." >&2
  exit 1
fi
log "Found capnp compiler."

# ---------- sanity ----------
if [[ ! -f "$SCHEMA" ]]; then
  log "Schema not found, skipping: $SCHEMA"
  exit 0
fi

# ---------- prepare outputs ----------
log "Creating output directories..."
vrun mkdir -p "$OUT_SRC" "$OUT_INCLUDE"
log "Output directories ready."

# ---------- optional clean ----------
if $CLEAN; then
  log "Cleaning previous generated artifacts..."
  vrun find "$OUT_SRC" -maxdepth 1 -type f -name '*.capnp.c++' -print -delete || true
  vrun find "$OUT_INCLUDE" -maxdepth 1 -type f -name '*.capnp.h'   -print -delete || true
fi

# ---------- temp build dir ----------
TMPDIR="$(mktemp -d /tmp/capnp_build.XXXXXX)"
trap 'rm -rf "$TMPDIR"' EXIT
log "Temporary directory: $TMPDIR"

# ---------- compile ----------
log "Compiling schema..."
vrun capnp compile \
  -oc++:"$TMPDIR" \
  --src-prefix="$SRC_DIR" \
  -I "$SRC_DIR" \
  "$SCHEMA"
log "Compilation completed."

# ---------- collect & move ----------
log "Scanning generated files under $TMPDIR..."
mapfile -t GEN_CPP < <(find "$TMPDIR" -type f -name '*.capnp.c++' | sort)
mapfile -t GEN_H   < <(find "$TMPDIR" -type f -name '*.capnp.h'   | sort)
log "Found ${#GEN_CPP[@]} .c++ and ${#GEN_H[@]} .h file(s)."

if (( ${#GEN_CPP[@]} == 0 && ${#GEN_H[@]} == 0 )); then
  log "WARNING: No generated files found. Check schema / options."
  exit 0
fi

if (( ${#GEN_CPP[@]} > 0 )); then
  log "Moving source file(s) to $OUT_SRC ..."
  for f in "${GEN_CPP[@]}"; do
    rel="${f#$TMPDIR/}"; log "  src: $rel -> $OUT_SRC/"
    vrun mv -f "$f" "$OUT_SRC/"
  done
else
  log "No .capnp.c++ files found."
fi

if (( ${#GEN_H[@]} > 0 )); then
  log "Moving header file(s) to $OUT_INCLUDE ..."
  for f in "${GEN_H[@]}"; do
    rel="${f#$TMPDIR/}"; log "  hdr: $rel -> $OUT_INCLUDE/"
    vrun mv -f "$f" "$OUT_INCLUDE/"
  done
else
  log "No .capnp.h files found."
fi

# ---------- retarget includes in generated .c++ ----------
# Rewrite:  #include "whatever.capnp.h"  ->  #include <messages/whatever.capnp.h>
if (( ${#GEN_CPP[@]} > 0 )); then
  log "Retargeting includes inside generated .c++ files to <messages/...> ..."
  changed=0
  shopt -s nullglob
  for f in "$OUT_SRC"/*.capnp.c++; do
    before_sum="$(md5sum "$f" | awk '{print $1}')"
    # Only replace simple quoted includes; keep system/other includes intact.
    # Takes the basename of the header if a path slipped in.
    vrun perl -0777 -pi -e '
      s{(^\s*#\s*include\s*)"(?:.*/)?([^"/]+\.capnp\.h)"}
       {$1<messages/$2>}mg;
    ' "$f"
    after_sum="$(md5sum "$f" | awk '{print $1}')"
    if [[ "$before_sum" != "$after_sum" ]]; then
      log "  updated: $(basename "$f")"
      ((changed++))
    fi
  done
  shopt -u nullglob
  log "Include retargeting done (${changed} file(s) updated)."
fi

# ---------- summary ----------
log "Cap'n Proto build done."
log "Headers in: $OUT_INCLUDE"
log "Sources in: $OUT_SRC"
echo "[capnp_build] Tip: add '${OUT_INCLUDE%/}/..' to your compiler include paths so '#include <messages/xxx.capnp.h>' works."