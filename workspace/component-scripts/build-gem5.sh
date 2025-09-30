#!/bin/bash

echo ""
echo "--------------"
echo "| Build gem5 |"
echo "--------------"
echo "Date and Time: " `date`
echo ""

# clone gem5
git clone 'https://github.com/gem5/gem5.git'
cd gem5

# checkout a version the patch is known to work with,
# and apply the patch
git checkout v25.0.0.1
git apply ../patches/gem5-patch

# install some git hooks -- we won't be making any commits,
# but the scons build stops and asks us to add them if they
# are missing
mv .git/hooks/commit-msg.sample .git/hooks/commit-msg
mv .git/hooks/pre-commit.sample .git/hooks/pre-commit

# build gem5
scons build/RISCV/gem5.opt -j`nproc`
