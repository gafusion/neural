#!/bin/bash

export NEURAL=$HOME/neural
export LD_LIBRARY_PATH=$HOME/fann/src

ln -sf $PWD/slice.py eped1nn/main.py
ln -sf $PWD/model.py eped1nn/model.py

ln -sf $PWD/slice.py tglfnn/main.py
ln -sf $PWD/model.py tglfnn/model.py

ln -sf $PWD/slice.py neojbsnn/main.py
ln -sf $PWD/model.py neojbsnn/model.py

sudo cp online_slice/index.html /usr/local/lib/python2.7/site-packages/bokeh/server/views/app_index.html
sudo cp online_slice/index.html /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages/bokeh/server/views/app_index.html

models=`ls -d */`
bokeh serve $models # --allow-websocket-origin gadb-harvest.ddns.net --port 5100
