========
EPED1-NN
========

There are different scripts to run the EPED1-NN model

Low level
---------
at the lowest level one can call the `brainfuse_run.exe` binary and pass the list
of NN files (generally called brainfuse_XX.net) and an input file. For example:

    cd eped1nn/samples
    ../../brainfuse_run.exe ../models/EPED1_H_superH input_sample.dat

here `EPED1_H_superH` is one of the models under the `models` directory

The `input_sample.dat` has the format of:

    N1
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    i1 i2 i3 i4 i5 i6 i7 i8 i9
    ...

where:

    N1 number of runs
    i.. inputs (N1 lines)

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

Python wrapper
--------------
The `eped1nn` Python script wraps the `brainfuse_run.exe` executable.
This Python script also generates a `epednn.profiles` file.

    cd eped1nn/samples
    ../eped1nn EPED1_H_superH input_sample.dat
    

Snyder EPED1 interface
----------------------
The `eped1nn_eped1` Python script mimics the I/O of Phil Snyder original IDL EPED1 worflow.
The format of the input is for example in `eped1nn/samples/2015check.txt`.

    cd eped1nn/samples
    ../eped1nn_eped1 EPED1_H_superH 2015check.txt

The output of the eped1nn_eped1 will append `out` to the input filename: `eped1nn/samples/2015check_out.txt`

============
TOQ-profiles
============
The EPED1 model uses model profiles within TOQ for the density and temperature profiles.
The TOQ routine for the profiles has been extracted and is available in `toq_profiles.f90`,
while the `toq_profiles_test.f90` is a sample driver showing how the routine is called.
