#!/bin/bash

export NEURAL=$PWD/..
export LD_LIBRARY_PATH=$PWD/../../fann/src

declare -a arr=("eped1nn-EPED1_H_superH" "eped1nn-D3D_RMP" "tglfnn" "neojbsnn-D_C" "neojbsnn-D_Z" "neojbsnn-DT_Z")

for i in "${arr[@]}"
do
    echo "$i"
    mkdir -p $i
    ln -sf $PWD/slice.py $i/main.py
    ln -sf $PWD/model.py $i/model.py

done

sudo cp index.html /usr/local/lib/python2.7/site-packages/bokeh/server/views/app_index.html
sudo cp index.html /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages/bokeh/server/views/app_index.html

models=`ls -d */`

export PATH=/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/:$PATH

#bokeh serve $models --allow-websocket-origin gadb-harvest.duckdns.org --port 5100
bokeh serve $models
