#!/usr/bin/env bash

set -eu

if [[ ! -c /dev/net/tun ]]; then
    echo "creating tun clonedev"
    mkdir /dev/net
    mknod /dev/net/tun c 10 200
    chmod 0666 /dev/net/tun
    modprobe tun
else
    echo "tun clonedev already exists"
fi

if [[ ! -a /sys/class/net/tun69 ]]; then
    echo "creating tun69"
    ip tuntap add dev tun69 mod tun
    ip addr add 10.0.0.1/24 dev tun69
    ip link set dev tun69 up
else
    echo "tun69 already exists"
fi

# https://docs.cyanlab.org/en/stable/CNIT481-SDN/source/labs/linux_bridges.html 
#down
#ip link set dev tun69 down

#deletion:
#ip tuntap del dev tun69 mode tun