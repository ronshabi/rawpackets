#!/usr/bin/env bash

set -euo pipefail

NAME=rawpackets

docker build -t "${NAME}" . 

docker run \
        -it \
        --mount type=bind,src=./code,dst=/code \
        -p 8080:80/tcp \
        -p 8080:80/udp \
        -p 8081:81/tcp \
        -p 8081:81/udp \
        "${NAME}"
