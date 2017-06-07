NEURAL repository
=================

This repository contains NN models for EPED1-NN, TGLF-NN and NEOjbs-NN

Refer to the `readme.md` files in the subfolders for more info.

Online widget
-------------

An online widget for exploring the models in this repository is available at http://gadb-harvest.ddns.net

Installation
------------

Install the FANN c library:

    git clone git@github.com:libfann/fann.git
    cd fann
    cmake .
    make

Install the Python bindings to the FANN library

    pip install fann2

Set in your .login file:

    setenv FANN_ROOT loation_where_fann_was_cloned

Run `./compile.sh` script

