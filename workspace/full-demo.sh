#!/bin/bash

echo ""
echo "-----------------------------------------------------"
echo "| Beginning full build of the Protmem demonstration |"
echo "-----------------------------------------------------"
echo "Date and Time: " `date`
echo ""

./component-scripts/build-toolchain.sh
./component-scripts/build-gem5.sh
./component-scripts/run-demo.sh

