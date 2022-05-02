NEURAL repository
=================

This repository contains NN models for EPED1-NN, TGLF-NN and NEOjbs-NN

Refer to the `readme.md` files in the subfolders for more info.

Two libraries have been used to train the NN models.

1. the FANN library is a NN library that allows little flexibility in the
definition onf the models, but has the advantage of being very portable,
and have bindings to many languages

2. the TENSORFLOW library allows great flexibility in building NN models,
and gives access to the most modern machine learning techniques and algorythmes,
but has the disadvantage of being more difficult to deploy the trained models
outside of Python. Yet running TENSORFLOW models from C, MATLAB, FORTRAN, should
be possible. For this purpose a client-server library that is capable of servicing
these models across the web over TCP/IP was developed. The client side of the library
is written in pure C (with available FORTRAN and Python interfaces) such that no
external library dependency is required. This approach lifts the cumbersome
installation requirements for doing inference with Tensorflow models from C.

We refer to as `BRAINFUSE` to the set of tools in this repository that are used to run these models.

FANN models
-----------

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


TENSORFLOW models
-----------------

To start one can use the public `brainfusetf` server `gadb-harvest.duckdns.org`
to serve trained models.

A python can be run with: `tf_client_server_test.py`

A C example can be run with:

    make brainfusetf_run.exe
    brainfusetf_run.exe