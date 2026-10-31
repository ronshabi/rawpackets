CC=gcc
CFLAGS="-g"

set -xeuo pipefail

rm -rfv build
mkdir -p build

$CC $CFLAGS main.c -o build/main