#!/bin/bash
export FANN_ROOT=/u/meneghini/fann
gcc -o brainfuse_run.exe brainfuse_run.c -O3 -L $FANN_ROOT/src -I $FANN_ROOT/src/include/ -lfloatfann -lm
