#include <stdio.h>
#include "brainfusetf_lib.h"

int main(int argc, char**argv){

  int i;
  char btf_sendline[65507];
  char model[256]="eped1nn/models/EPED_mb_128_pow_norm_common_30x10.pb";
  double input[10]={0.5778, 1.8034, 2.0995, 0.2075, 1.1621, 1.8017, 2, 4.0101, 1.6984, 1.4429};
  double output[18];

  btf_run(model, input, sizeof(input)/sizeof(double), output, sizeof(output)/sizeof(double));
  for (i=0; i<sizeof(output)/sizeof(output[0]); i++)
    printf("%f\n",output[i]);
}
