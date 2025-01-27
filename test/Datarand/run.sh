#!/bin/bash

rm *.png *.dot
file="$1"
fileinst=$file"_inst"

set -x

export LLVMSRC=/morespace/data-only-attack-mitigation
export LLVMROOT=/morespace/data-only-attack-mitigation/build/bin
export glibc_install=/mnt/Projects/tpalit/glibc/glibc-install

rm null_helper.c aes_inreg.s aes_inmemkey.s aes_helper.c internal_libc.c
#LLVMROOT=/mnt/donotuse_comparisonONLY/DataRandomization/install/bin

ln -s $LLVMSRC/lib/Transforms/Encryption/null_helper.c_ null_helper.c
ln -s $LLVMSRC/lib/Transforms/Encryption/aes_inmemkey.s aes_inmemkey.s
ln -s $LLVMSRC/lib/Transforms/Encryption/aes_inreg.s aes_inreg.s
ln -s $LLVMSRC/lib/Transforms/Encryption/aes_helper.c_ aes_helper.c
ln -s $LLVMSRC/lib/Transforms/LibcTransform/internal_libc.c_ internal_libc.c

GGDB=-ggdb 
$LLVMROOT/clang -O0 -c $GGDB  -emit-llvm $file.c -o $file.bc
if [ $? -ne 0 ]
then
    exit 1
fi

$LLVMROOT/clang -c $GGDB -emit-llvm internal_libc.c -o internal_libc.bc
if [ $? -ne 0 ]
then
    exit 1
fi


$LLVMROOT/llvm-link $file.bc internal_libc.bc  -o $file.bc #internal_libc.bc
if [ $? -ne 0 ]
then
    exit 1
fi

$LLVMROOT/opt -encryption  $file.bc -o $fileinst.bc 
$LLVMROOT/llvm-dis $file.bc -o $file.ll
$LLVMROOT/llvm-dis $fileinst.bc -o $fileinst.ll

$LLVMROOT/llc -O0 -filetype=obj $fileinst.bc -o $fileinst.o
if [ $? -ne 0 ]
then
    exit 1
fi

$LLVMROOT/clang -c -O0  -DLLI -fPIC -fPIE $SANITIZE $GGDB aes_helper.c -o aes_h.o
$LLVMROOT/clang -c -O0 $GGDB -fPIC -fPIE aes_inreg.s -o aes.o

./run_glibc.sh $fileinst.o $file
if [ $? -ne 0 ]
then
    exit 1
fi


