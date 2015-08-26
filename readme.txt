#1) get, make, make install of the FANN library: git@github.com:libfann/fann.git
#2) run ./compile.sh script
#3) run NEUPED with:
    
    neuped model_name input_run_file

where possible `model_name` are: `EPED1_D3D`, `EPED1_ITER`
and the `input_run_file` has the format of:

N1 N2 N3
i1 i2 i3 i4 i5 i6 i7 i8 i9
o1 o2 o3 o4
i1 i2 i3 i4 i5 i6 i7 i8 i9
o1 o2 o3 o4
...

where:
N1 number of runs
N2 number of inputs
N3 number of outputs
i.. inputs (N1 lines)
o.. ouputs (N1 lines, entries must be there but they are ignored)

upon run the `output_avg.dat` and `output_std.dat` files will be generated
with the same format of the `input_run_file` and will contain the average
value of the prediction and the standard deviation of the prediction,
respectively where the outputs field is. Notice that the `sample` directory
contains samples of the `input_run_file` for each of the models.

The inputs/outputs for the different models are:
=========
EPED1_D3D
=========
input_names='a' 'betan' 'bt' 'delta' 'ip' 'kappa' 'neped' 'r' 'zeffped'
output_names='OUT_p_E1' 'OUT_ptop_E1' 'OUT_wid_E1' 'OUT_widtop_E1'

==================
EPED1_ITERbaseline
==================
input_names='betan' 'delta' 'kappa' 'neped' 'zeffped'
output_names='OUT_p_E1' 'OUT_ptop_E1' 'OUT_wid_E1' 'OUT_widtop_E1'
