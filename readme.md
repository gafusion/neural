NEUPED
======

Engine for neural-network based pedestal predictions

Getting started
---------------
Run NEUPED with:
    
    neuped model_name input_run_file

For example:

    neuped EPED1_D3D samples/EPED1_D3D/input.dat

Pedestal models
---------------
Possible `model_name` are: `EPED1_D3D`, `EPED1_ITERbaseline`

* EPED1_D3D
 * assumes: `zeta=0.0` `m=2.0` `z=1`
 * inputs: `a` `betan` `bt` `delta` `ip` `kappa` `neped` `r` `zeffped`
 * outputs: `pped` `ptop` `wped` `wtop`

* EPED1_ITERbaseline
 * assumes: `ip=15.0` `bt=5.3` `r=6.2` `a=2` `zeta=0.0` `betan=2.0` `m=2.5` `z=1`
 * inputs: `betan` `delta` `kappa` `neped` `zeffped`
 * outputs: `pped` `ptop` `wped` `wtop`

Inputs format
-------------
Notice that the `sample` directory contains samples of the `input_run_file` for each of the models.
The `input_run_file` has the format of:

    N1
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    ...

where:

    N1 number of runs
    i.. inputs (N1 lines)

Outputs format
--------------
Upon run the `output_avg.dat` and `output_std.dat` files will be generated in the
current working directory with the NEUPED prediction and its standard deviation.
Both files have format:

    N1
    o1 o2 o3 o4
    o1 o2 o3 o4
    ...

where::

    N1 number of runs
    o.. outputs (N1 lines)

The `input_lim.dat` file contains the values of the inputs normalized to the training range
(assuming a normal distribution of the training data). The values in this file cat be used
to check that values provided by the user are within the training range of the model. e.g:

    +1.0 means the input value is +1.0 standard deviation away from the mean of the training range
    -1.3 means the input value is -1.3 standard deviation away from the mean of the training range
    -0.0 means the input value is right in the middle (mean) of the training range

A rule of thumb is that the model is applicable where the magnitude of the normalized inputs is <2

The `input_lim.dat` file has format:

    N1
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    ...

where:

    N1 number of runs
    i.. inputs (N1 lines)

Installation
------------
* Clone and cmake install the FANN library: `git@github.com:libfann/fann.git`

* Modify and run ./compile.sh script
