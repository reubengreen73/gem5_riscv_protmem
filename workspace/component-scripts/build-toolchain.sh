#!/bin/bash

echo ""
echo "-------------------------"
echo "| Build RISCV toolchain |"
echo "-------------------------"
echo "Date and Time: " `date`
echo ""

# clone the toolchain
git clone 'https://github.com/riscv-collab/riscv-gnu-toolchain.git'
cd riscv-gnu-toolchain

# checkout a version this patch is known to work with
git checkout 2025.09.16

# apply the Protmem patch in binutils
git submodule update --init binutils
cd binutils
git apply ../../patches/binutils-patch
cd ..

# configure and build
./configure --prefix=/opt/riscv-protmem --host=riscv64-unknown-elf
make -j`nproc`
