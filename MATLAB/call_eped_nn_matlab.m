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
system('sh /Users/fusionpower/Documents/Neural_Networks/TGLF_NN/test.sh');

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
outputs = Call_EPED_NN(inputs)

% Extract outputs
% o1 = outputs(1);
% o2 = outputs(2);
% o3 = outputs(3);
% o4 = outputs(4);
% o5 = outputs(5);
% o6 = outputs(6);
% o7 = outputs(7);
% o8 = outputs(8);
% o9 = outputs(9);
% o10 = outputs(10);
% o11 = outputs(11);
% o12 = outputs(12);
% o13 = outputs(13);
% o14 = outputs(14);
% o15 = outputs(15);
% o16 = outputs(16);
% o17 = outputs(17);
% o18 = outputs(18);

format long
e = cputime - t


