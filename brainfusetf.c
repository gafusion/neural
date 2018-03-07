#include <stdio.h>
#include "brainfusetf_lib.h"

int main(int argc, char**argv){

  int i;
  char btf_sendline[65507];
  char model[256]="tglfnn/models/nn_SAT1_mb_1024_abs_reg_common_stair2x2x6.pb";
  double input[21]={5.45338e-01,  4.17925e-02,  7.21986e-03,  1.24956e-01, -1.37899e-01,  1.58491e+00, -4.20101e-03,  1.55640e+00, 8.36932e+00,  1.02569e+00,  2.05921e+00, -4.45231e-01, 3.00670e+00,  2.06023e+00,  2.38220e+00,  7.66336e-01, 3.20824e-01,  1.14110e+00,  3.21049e-01,  3.36619e-01, 1.87324e+00};
  double output[6];

  btf_run(model,input,sizeof(input)/sizeof(double),output,sizeof(output)/sizeof(double));
  for (i=0; i<6; i++)
    printf("%f\n",output[i]);
}

