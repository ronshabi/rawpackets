#!/bin/bash

# TODO: I need a script to sync all of the configs from there to here.

export TERM=xterm-256color
export PS1='\u@\h (\w) > QEMU: '
alias ZR='source ~/.bashrc'
alias ll='ls -lahF --color=auto'

m() { mkdir -p /code && mount -t 9p -o trans=virtio,version=9p2000.u code /code; }
