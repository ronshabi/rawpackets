#!/usr/bin/env bash

set -xeuo pipefail

ALPINE="$HOME/000/Linux/alpine-standard-3.24.3-aarch64.iso"
IMG_NAME=qemu/alpine.qcow2
IMG_SIZE=20G

QPATH="/opt/homebrew/Cellar/qemu/11.1.3/share/qemu"
EDK2="$QPATH/edk2-aarch64-code.fd"
EDK2VARS="$QPATH/edk2-arm-vars.fd"
EDK2VARS_LOCAL="qemu/edk2-arm-vars.fd"


[[ -f $EDK2VARS_LOCAL ]] || cp "$EDK2VARS" "$EDK2VARS_LOCAL"


# qemu-img -f qcow2 create $IMG_NAME $IMG_SIZE

    # -cdrom $ALPINE \
    # -boot order=d \

qemu-system-aarch64 \
    -machine virt \
    -cpu host \
    -accel hvf \
    -smp 1 \
    -m 1G \
    -drive file=$EDK2,if=pflash,format=raw,readonly=on \
    -drive file=$EDK2VARS_LOCAL,if=pflash,format=raw \
    -drive file=$IMG_NAME,if=virtio,format=qcow2 \
    -device virtio-net-pci,netdev=net0 \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -nographic \
    -virtfs local,path="$PWD/code",mount_tag=code,security_model=mapped-xattr

# mkdir -p /code && mount -t 9p -o trans=virtio,version=9p2000.u code /code
# https://wiki.qemu.org/Documentation/9psetup