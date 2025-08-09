#!/usr/bin/env bash
# Build the 'networkparser' target with CMake, then run it on schemas/network.dsl.
# Generated files go to:
#   *.cpp  -> OUT_SRC
#   *.h    -> OUT_INCLUDE
# and we rewrite #include "X.h" -> #include <network/X.h> in generated .cpp files.
#
# Usage:
#   ./network_build.sh [--src DIR] [--schema FILE] [--out-src DIR] [--out-include DIR] [-v] [--clean]
#
# Defaults:
#   --src          $PWD/
#   --schema       <src>/schemas/network.dsl
#   --out-src      $PWD/src/network/src
#   --out-include  $PWD/include/network

set -euo pipefail

# ---------- defaults ----------
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

SRC_DIR="$PWD/"
SCHEMA=""
OUT_SRC="$PWD/src/network/src"
OUT_INCLUDE="$PWD/include/network"
VERBOSE=false
CLEAN=false

log() { echo "[network_build] $*"; }
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
  ROOT_DIR       $ROOT_DIR
  BUILD_DIR      $BUILD_DIR
  --src          $SRC_DIR
  --schema       <src>/schemas/network.dsl
  --out-src      $OUT_SRC
  --out-include  $OUT_INCLUDE
EOF
      exit 0 ;;
    *) echo "Unknown arg: $1" >&2; exit 1 ;;
  esac
done

# ---------- resolve schema default ----------
if [[ -z "$SCHEMA" ]]; then
  SCHEMA="${SRC_DIR%/}/schemas/network.dsl"
fi

# ---------- banner ----------
log "Root dir:        $ROOT_DIR"
log "Build dir:       $BUILD_DIR"
log "Source dir:      $SRC_DIR"
log "Output src dir:  $OUT_SRC"
log "Output include:  $OUT_INCLUDE"
log "Schema file:     $SCHEMA"

# ---------- sanity ----------
if [[ ! -f "$ROOT_DIR/CMakeLists.txt" ]]; then
  echo "[network_build][ERR] No CMakeLists.txt at $ROOT_DIR (run this from repo root)." >&2
  exit 1
fi
if [[ ! -f "$SCHEMA" ]]; then
  echo "[network_build][ERR] Schema not found: $SCHEMA" >&2
  exit 1
fi

# ---------- build 'networkparser' with CMake ----------
log "Configuring CMake project (in $BUILD_DIR)..."
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR" >/dev/null
  vrun cmake ..
  log "Building target 'networkparser'..."
  # Use cmake --build for generator-agnostic builds
  vrun cmake --build . --target networkparser -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)"
popd >/dev/null

# ---------- locate networkparser executable ----------
NETP_BIN=""
CANDIDATES=(
  "$ROOT_DIR/releases/networkparser"
  "$BUILD_DIR/networkparser"
  "$BUILD_DIR/src/network/networkparser"
)
for c in "${CANDIDATES[@]}"; do
  [[ -x "$c" ]] && NETP_BIN="$c" && break
done
if [[ -z "$NETP_BIN" ]]; then
  # last resort: search build tree
  found="$(find "$BUILD_DIR" -type f -name networkparser -perm -111 2>/dev/null | head -n 1 || true)"
  [[ -n "$found" ]] && NETP_BIN="$found"
fi
if [[ -z "$NETP_BIN" ]]; then
  echo "[network_build][ERR] Could not find built 'networkparser' binary." >&2
  echo "Checked: ${CANDIDATES[*]}" >&2
  exit 1
fi
log "Using networkparser: $NETP_BIN"

# ---------- prepare outputs ----------
log "Creating output directories..."
vrun mkdir -p "$OUT_SRC" "$OUT_INCLUDE"
log "Output directories ready."

# ---------- optional clean ----------
if $CLEAN; then
  log "Cleaning previous generated artifacts..."
  vrun find "$OUT_SRC" -maxdepth 1 -type f -name '*.cpp' -print -delete || true
  vrun find "$OUT_INCLUDE" -maxdepth 1 -type f -name '*.h' -print -delete || true
fi

# ---------- temp build dir ----------
TMPDIR="$(mktemp -d /tmp/network_build.XXXXXX)"
trap 'rm -rf "$TMPDIR"' EXIT
log "Temporary directory: $TMPDIR"

# ---------- compile DSL ----------
log "Compiling schema with networkparser..."
# Expected CLI: networkparser <schema> --out <dir>
# If your tool uses different flags, adjust below.
vrun "$NETP_BIN" "$SCHEMA" "$TMPDIR"
log "Compilation completed."

# ---------- collect & move ----------
log "Scanning generated files under $TMPDIR..."
mapfile -t GEN_CPP < <(find "$TMPDIR" -type f -name '*.cpp' | sort)
mapfile -t GEN_H   < <(find "$TMPDIR" -type f -name '*.h'   | sort)
log "Found ${#GEN_CPP[@]} .cpp and ${#GEN_H[@]} .h file(s)."

if (( ${#GEN_CPP[@]} == 0 && ${#GEN_H[@]} == 0 )); then
  log "WARNING: No generated files found. Check schema / generator output."
  exit 0
fi

if (( ${#GEN_CPP[@]} > 0 )); then
  log "Moving source file(s) to $OUT_SRC ..."
  for f in "${GEN_CPP[@]}"; do
    rel="${f#$TMPDIR/}"; log "  src: $rel -> $OUT_SRC/"
    vrun mv -f "$f" "$OUT_SRC/"
  done
else
  log "No .cpp files found."
fi

if (( ${#GEN_H[@]} > 0 )); then
  log "Moving header file(s) to $OUT_INCLUDE ..."
  for f in "${GEN_H[@]}"; do
    rel="${f#$TMPDIR/}"; log "  hdr: $rel -> $OUT_INCLUDE/"
    vrun mv -f "$f" "$OUT_INCLUDE/"
  done
else
  log "No .h files found."
fi

# ---------- retarget includes in generated .cpp ----------
# Rewrite:  #include "whatever.h"  ->  #include <network/whatever.h>
if (( ${#GEN_CPP[@]} > 0 )); then
  log "Retargeting includes inside generated .cpp files to <network/...> ..."
  changed=0
  shopt -s nullglob
  for f in "$OUT_SRC"/*.cpp; do
    before_sum="$(md5sum "$f" | awk '{print $1}')"
    vrun perl -0777 -pi -e '
      s{(^\s*#\s*include\s*)"(?:.*/)?([^"/]+\.h)"}
       {$1<network/$2>}mg;
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
log "Network build done."
log "Headers in: $OUT_INCLUDE"
log "Sources in: $OUT_SRC"
echo "[network_build] Tip: add '${OUT_INCLUDE%/}/..' to your compiler include paths so '#include <network/xxx.h>' works."