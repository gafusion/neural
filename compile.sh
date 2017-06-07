#!/bin/bash

make clean
make brainfuse_run.exe toq_profiles_test

cd tglfnn/samples
rm -f output.avg
./run_test.sh
cat output.avg