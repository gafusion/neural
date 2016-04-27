#!/bin/bash
#cp ../brainfuse/brainfuse_run.c ./
gcc -o brainfuse_run.exe brainfuse_run.c -O3 -L $FANN_ROOT/src -I $FANN_ROOT/src/include/ -lfloatfann -lm
