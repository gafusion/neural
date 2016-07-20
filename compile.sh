#!/bin/bash
#cp ../brainfuse/brainfuse_run.c ./

make clean
make brainfuse_run.exe

cd tglfnn/samples
rm -f output.avg
./run_test.sh
cat output.avg