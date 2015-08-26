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
 * input_names = 'a' 'betan' 'bt' 'delta' 'ip' 'kappa' 'neped' 'r' 'zeffped'
 * output_names = 'OUT_p_E1' 'OUT_ptop_E1' 'OUT_wid_E1' 'OUT_widtop_E1'

* EPED1_ITERbaseline
 * input_names = 'betan' 'delta' 'kappa' 'neped' 'zeffped'
 * output_names = 'OUT_p_E1' 'OUT_ptop_E1' 'OUT_wid_E1' 'OUT_widtop_E1'

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

Installation
------------
* Clone and cmake install the FANN library: `git@github.com:libfann/fann.git`

* Modify and run ./compile.sh script
