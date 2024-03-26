#!/bin/bash

echo deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main >> /etc/apt/sources.list
echo deb-src http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main >> /etc/apt/sources.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -
apt update
apt upgrade -y
apt install clang-11 llvm-11 llvm-11-dev lldb-11 lld-11 -y
apt install libc++-11-dev libc++abi-11-dev -y
ln -s /usr/bin/clang-11 /usr/bin/clang
ln -s /usr/bin/clang++-11 /usr/bin/clang++
ln -s /usr/bin/llvm-ar-11 /usr/bin/llvm-ar
ln -s /usr/bin/llvm-as-11 /usr/bin/llvm-as
ln -s /usr/bin/llvm-config-11 /usr/bin/llvm-config
ln -s /usr/bin/opt-11 /usr/bin/opt
