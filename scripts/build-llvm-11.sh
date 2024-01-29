#!/bin/bash

sudo echo deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-11 main >> /etc/apt/sources.list
sudo echo deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-11 main >> /etc/apt/sources.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
sudo apt update
sudo apt upgrade
sudo apt install clang-11 llvm-11 llvm-11-dev lldb-11 lld-11 -y
sudo apt install libc++-11-dev libc++abi-11-dev -y
sudo ln -s /usr/bin/clang-11 /usr/bin/clang
sudo ln -s /usr/bin/clang++-11 /usr/bin/clang++
sudo ln -s /usr/bin/llvm-ar-11 /usr/bin/llvm-ar
sudo ln -s /usr/bin/llvm-as-11 /usr/bin/llvm-as
sudo ln -s /usr/bin/llvm-config-11 /usr/bin/llvm-config
