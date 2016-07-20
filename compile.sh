#!/bin/bash
#cp ../brainfuse/brainfuse_run.c ./

rm -f brainfuse_run.exe

gcc -c brainfuse_lib.c -O3
gcc -o brainfuse_run.exe brainfuse_run.c brainfuse_lib.o -O3 -L $FANN_ROOT/src -I $FANN_ROOT/src/include/ -lfloatfann -lm

cd tglfnn/samples
rm -f output.avg
./run_test.sh
cat output.avg