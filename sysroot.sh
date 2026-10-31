#!/usr/bin/env bash

set -euo pipefail

ID="$1"
DEST=./sysroot

rm -rf "${DEST}"
mkdir -p "${DEST}/usr"
docker exec "${ID}" tar -C / -chf - usr/include | tar -C "${DEST}" -xf -
find sysroot -type l
du -h sysroot