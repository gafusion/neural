#include <stdio.h>
#include "brainfuse_lib.h"

//============
// MAIN
//============
int main(int argc, char *argv[])
{
  // Check input consistency
  if (argc < 3)
    {
      printf("Usage: %s ANN_file[s] run_file\n", argv[0]);
      return -1;
    }

  // Read in input parameters
  unsigned int j,n,num_data;
  fann_type *data_in;
  const char *runFile = argv[argc-1];
  FILE *fp1, *fp2;

  // Initialize arrays
  load_anns(argc-2,argv+1);

  // Read input data
  printf("Reading data from file %s: ", runFile);
  fp1 = fopen(runFile, "r");
  fscanf(fp1, "%u\n", &num_data);
  printf("%u runs %d inputs %d ouputs\n", num_data, get_anns_num_input(), get_anns_num_output());
  data_in = malloc(get_anns_num_input() * sizeof(fann_type));
  for(j = 0; j < get_anns_num_input(); j++){
    fscanf(fp1, FANNSCANF " ", &data_in[j]);
  }
  fclose(fp1);

  // Load inputs
  load_anns_inputs(data_in);

  // Run anns
  run_anns();

  // print and write
  fp1 = fopen("output.avg", "w");
  fp2 = fopen("output.std", "w");
  fprintf(fp1,"%u\n",num_data);
  fprintf(fp2,"%u\n",num_data);
  for(j = 0; j < get_anns_num_output(); j++){
      printf("%f (%f) ",get_anns_avg()[j],get_anns_std()[j] );
      fprintf(fp1,"%f ",get_anns_avg()[j]);
      fprintf(fp2,"%f ",get_anns_std()[j]);
  }
  printf("\n");
  fprintf(fp1,"\n");
  fprintf(fp2,"\n");
  fclose(fp1);
  fclose(fp2);
  return 0;
}