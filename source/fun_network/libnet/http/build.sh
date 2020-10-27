#!/bin/sh

rm a.out
gcc -I . -I ../include  -L ../lib/x86_64 *.c  -lnet

export LD_LIBRARY_PATH=../lib/x86_64/
