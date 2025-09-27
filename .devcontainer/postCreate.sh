#!/bin/bash

# Update package lists
sudo apt-get update

# Install essential build tools
sudo apt-get install -y \
    build-essential \
    gcc \
    g++ \
    gdb \
    clang \
    make \
    cmake \
    autoconf \
    automake \
    libtool \
    valgrind \
    git \
    vim \
    bin86 \
    nano

# Install additional tools for kernel development
sudo apt-get install -y \
    qemu-system-x86 \
    xorriso \
    grub-pc-bin \
    grub-efi-amd64-bin

# Create build directory
mkdir -p /workspaces/Cmake-linux0.11/build

echo "Development environment setup complete!"
echo "Build directory created at: /workspaces/Cmake-linux0.11/build"