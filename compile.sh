#!/bin/bash
#cp ../brainfuse/brainfuse_run.c ./

ln -sf online_slice/slice.py online_slice/eped1nn/main.py
ln -sf online_slice/model.py online_slice/eped1nn/model.py

ln -sf online_slice/slice.py online_slice/tglfnn/main.py
ln -sf online_slice/model.py online_slice/tglfnn/model.py

ln -sf online_slice/slice.py online_slice/neojbsnn/main.py
ln -sf online_slice/model.py online_slice/neojbsnn/model.py

make clean
make brainfuse_run.exe

cd tglfnn/samples
rm -f output.avg
./run_test.sh
cat output.avg