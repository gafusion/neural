%% Eped FANN Model
clc
clear all

t = cputime;

% Calculate input values
a = 0.5778;
betan = 1.8034;
bt = 2.0995;
delta = 0.2075;
ip = 1.1621;
kappa = 1.8017;
m = 2;
neped = 4.0101;
r = 1.6984;
zeffped = 1.4429;

inputs = [a betan bt delta ip kappa m neped r zeffped];

% Write inputs to file
input_file = fopen('inputs.dat', 'w');
fprintf(input_file, '%f\n', 1);
fprintf(input_file, '%f\n', inputs);
fclose(input_file);

% Run neural network
% Figure out fann.h files to run fann model
system('sh eped_fann_test.sh');

output_file = fopen('output.avg', 'r');
output_avg = fscanf(output_file, '%f\n');
fclose(output_file);

% Extract outputs
OUT_p_E1_0 = output_avg(2);
OUT_p_E1_2 = output_avg(3);
OUT_wid_E1_0 = output_avg(4);
OUT_wid_E1_2 = output_avg(5);

e = cputime - t


%% Eped TENSORFLOW Model
clc
clear all

% Generate MEX function
% Run once
% mex Call_EPED_NN.c

t = cputime;

% Calculate input values
a = 0.5778;
betan = 1.8034;
bt = 2.0995;
delta = 0.2075;
ip = 1.1621;
kappa = 1.8017;
m = 2;
neped = 4.0101;
r = 1.6984;
zeffped = 1.4429;

inputs = [a betan bt delta ip kappa m neped r zeffped];

% Run neural network
outputs = Call_EPED_NN(inputs);

% Extract outputs
o1 = outputs(1);
o2 = outputs(2);
o3 = outputs(3);
o4 = outputs(4);
o5 = outputs(5);
o6 = outputs(6);
o7 = outputs(7);
o8 = outputs(8);
o9 = outputs(9);
o10 = outputs(10);
o11 = outputs(11);
o12 = outputs(12);
o13 = outputs(13);
o14 = outputs(14);
o15 = outputs(15);
o16 = outputs(16);
o17 = outputs(17);
o18 = outputs(18);

format long
e = cputime - t


%% TGLF TENSORFLOW Model
clc
clear all

% Generate MEX function
% Run once
% mex Call_TGLF_NN.c

t = cputime;

% Calculate input values
as_2 = 2.52925;
as_3 = 0.00205474;
betae = 0.195821;
debye = 0.015207;
delta_loc = 0.10706;
drmajdx_loc = 0.709042;
kappa_loc = 0.00879205;
p_prime_loc = 0.0509749;
q_loc = 0.0837711;
q_prime_loc = 0.0153145;
rlns_1 = 1.40512;
rlns_2 = -0.05166936;
rlns_3 = 1.17357;
rlts_1 = -0.0039616;
rlts_2 = 0.512115;
rmaj_loc = 5.26647;
rmin_loc = 0.59511;
s_kappa_loc = 0.44139;
taus_2 = 1.87827;
vexb_shear = 1.78045;
vpar_1 = 0.510876;
vpar_shear_1 = 3.22174;
xnue = 1.01135;
zeff = 0.107648;


inputs = [as_2 as_3 betae debye delta_loc drmajdx_loc kappa_loc p_prime_loc q_loc q_prime_loc...
    rlns_1 rlns_2 rlns_3 rlts_1 rlts_2 rmaj_loc rmin_loc s_kappa_loc taus_2 vexb_shear vpar_1...
    vpar_shear_1 xnue zeff];

% Run neural network
outputs = Call_TGLF_NN(inputs)

% Extract outputs
% energy_flux_1 = outputs(1);
% energy_flux_i = outputs(2);
% particle_flux_1 = outputs(3);
% particle_flux_2 = outputs(4);
% particle_flux_3 = outputs(5);
% stress_tor_i = outputs(6);

format long
e = cputime - t


