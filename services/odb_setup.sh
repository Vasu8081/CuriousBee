#!/bin/bash

set -e

# -------- CONFIGURABLE VARIABLES --------
UBUNTU_VER="${1:-24.04}"        # Pass as first argument, default 24.04
ODB_VER="${2:-2.5.0}"           # Pass as second argument, default 2.5.0
ARCH="${3:-x86_64}"             # Pass as third argument, default x86_64

# Choose which adapters to install: (mysql, pgsql, sqlite, qt, boost)
# Use "all" for everything, or a space-separated list (e.g., "pgsql sqlite")
ADAPTERS="${4:-pgsql}"            # Pass as fourth argument

BASE_URL="https://www.codesynthesis.com/download/odb/${ODB_VER}/ubuntu/ubuntu${UBUNTU_VER}/${ARCH}"

CORE_FILES=(
  "odb_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
  "libodb_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
  "libodb-dev_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
)

declare -A ADAPTER_FILES
ADAPTER_FILES[mysql]="libodb-mysql_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb libodb-mysql-dev_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
ADAPTER_FILES[pgsql]="libodb-pgsql_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb libodb-pgsql-dev_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
ADAPTER_FILES[sqlite]="libodb-sqlite_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb libodb-sqlite-dev_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
ADAPTER_FILES[qt]="libodb-qt_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb libodb-qt-dev_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"
ADAPTER_FILES[boost]="libodb-boost_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb libodb-boost-dev_${ODB_VER}-0~ubuntu${UBUNTU_VER}_amd64.deb"


# ------------- ARG PARSING HELP -------------
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
  echo "Usage: $0 [UBUNTU_VER] [ODB_VER] [ARCH] [ADAPTERS]"
  echo "Examples:"
  echo "  $0                    # Defaults to 24.04, 2.5.0, x86_64, all adapters"
  echo "  $0 24.04 2.5.0        # Ubuntu 24.04, ODB 2.5.0, all adapters"
  echo "  $0 22.04 2.5.0 x86_64 pgsql sqlite"
  echo "  $0 24.04 2.5.0 x86_64 mysql"
  exit 0
fi

# --------- CHOOSE ADAPTERS ----------
ALL_ADAPTERS=(mysql pgsql sqlite qt boost)
if [[ "$ADAPTERS" == "all" ]]; then
  ADAPTERS="${ALL_ADAPTERS[*]}"
fi

FILES=("${CORE_FILES[@]}")
for adp in $ADAPTERS; do
  FILES+=(${ADAPTER_FILES[$adp]})
done

echo "--------------------------------------"
echo "ODB install script:"
echo " Ubuntu version:  $UBUNTU_VER"
echo " ODB version:     $ODB_VER"
echo " Architecture:    $ARCH"
echo " Adapters:        $ADAPTERS"
echo "--------------------------------------"

TMPDIR="odb_install_tmp_${RANDOM}"
mkdir -p "$TMPDIR"
cd "$TMPDIR"

echo "Downloading ODB packages from $BASE_URL ..."
for file in "${FILES[@]}"; do
  if [ ! -f "$file" ]; then
    echo "  Downloading $file"
    wget -q --show-progress "$BASE_URL/$file"
  fi
done

echo "Installing packages with dpkg..."
sudo dpkg -i *.deb || sudo apt-get -f install -y

cd ..
rm -rf "$TMPDIR"

echo "All done! ODB ($ODB_VER) and selected adapters are installed."