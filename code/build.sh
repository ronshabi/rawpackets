CC=gcc
CFLAGS="-g"

set -euo pipefail
# set -x

rm -rf build
mkdir -p build

BINARY="rawpackets"


for c_source in *.c; do
    c_obj=${c_source/%.c/.o}
    echo "  CC    $c_obj"
    $CC $CFLAGS "$c_source" -c -o "build/$c_obj" 
done

# Link
echo "LINK    $BINARY"
$CC build/*.o -o build/$BINARY