FROM debian

ENV TERM=xterm-256color
RUN echo "alias ll='ls -lahF --color=auto'" >> /etc/bash.bashrc
EXPOSE 80/tcp 80/udp 81/tcp 81/udp


RUN apt update && apt install -y python3 git curl wget tcpdump net-tools ethtool\
    neovim dnsutils ldnsutils dnsmasq build-essential binutils gdb lua5.4 \
    linux-headers-arm64 dkms htop tmux manpages man-db ripgrep \
    bear clang clang-format clang-tidy 

WORKDIR /code

CMD ["bash"]