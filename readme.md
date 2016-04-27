EPED1NN
======

Engine for neural-network powered pedestal predictions

Getting started
---------------
Run EPED1NN with:
    
    eped1nn model_name input.dat

For example:

    cd samples
    ../eped1nn_eped1 EPED1_D3D 2015check.txt

Pedestal models
---------------
Possible `model_name` are the directory names contained in the models directory
e.g. EPED1_ALL

raw inputs format
-------------
The `input.dat` has the format of:

    N1
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    ...

where:

    N1 number of runs
    i.. inputs (N1 lines)

EPED1 input format
------------------
The `eped1nn_eped1` script will automatically make the conversion from
the EPED1 input file format to the EPED1NN file format.

    eped1nn_epde1 EPED1_D3D eped1_input_file

raw outputs format
--------------
Upon run the `output.dat` and `output.std` files will be generated in the
current working directory with the EPED1NN prediction and its standard deviation.
Both files have format:

    N1
    o1 o2 o3 o4
    o1 o2 o3 o4
    ...

where::

    N1 number of runs
    o.. outputs (N1 lines)

The `input.lim` file contains the values of the inputs normalized to the training range
(assuming a normal distribution of the training data). The values in this file cat be used
to check that values provided by the user are within the training range of the model. e.g:

    +1.0 means the input value is +1.0 standard deviation away from the mean of the training range
    -1.3 means the input value is -1.3 standard deviation away from the mean of the training range
    -0.0 means the input value is right in the middle (mean) of the training range

A rule of thumb is that the model is applicable where the magnitude of the normalized inputs is <2

The `input.lim` file has format:

    N1
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    ...

where:

    N1 number of runs
    i.. inputs (N1 lines)

On GA workstations
------------------
Set in your .login file

    setenv FANN_ROOT /u/meneghini/fann

Installation
------------
Install the FANN library:

    git clone git@github.com:libfann/fann.git
    cd fann
    cmake .
    make

Set in your .login file:

    setenv FANN_ROOT loation_where_fann_was_cloned

Run `./compile.sh` script in the EPED1NN folder
