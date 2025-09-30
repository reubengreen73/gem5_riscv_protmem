#!/bin/bash

echo ""
echo "------------------------------"
echo "| Build and run demo program |"
echo "------------------------------"
echo "Date and Time: " `date`
echo ""

# build the demo program
cd demo-program
/opt/riscv-protmem/bin/riscv64-unknown-elf-gcc protmem-demo.c -o protmem-demo
cd ../gem5

# run the demo program in gem5
./build/RISCV/gem5.opt ../demo-program/protmem-demo-config.py
