#include <stdio.h>
#include <brainfuse_lib.h>

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
  const char *annFiles = argv[argc-1];
  FILE *fi, *fa, *fs;

  // Initialize arrays
  load_anns(0,argv[1],"brainfuse");

  // Open I/O files
  printf("Reading data from file %s: ", runFile);
  fi = fopen(runFile, "r");
  fscanf(fi, "%u\n", &num_data);
  fa = fopen("output.avg", "w");
  fs = fopen("output.std", "w");
  fprintf(fa,"%u\n",num_data);
  fprintf(fs,"%u\n",num_data);

  // Print info
  printf("%u runs %d inputs %d ouputs\n", num_data, get_anns_num_input(), get_anns_num_output());

  // Initialize array
  data_in = malloc(get_anns_num_input() * sizeof(fann_type));
  for(n = 0; n < num_data; n++){

      // Read input data
      for(j = 0; j < get_anns_num_input(); j++){
        fscanf(fi, FANNSCANF " ", &data_in[j]);
      }

      // Load inputs
      load_anns_inputs(data_in);

      // Run anns
      run_anns();

      // print and write
      for(j = 0; j < get_anns_num_output(); j++){
          printf("%f (%f) ",get_anns_avg(j), get_anns_std(j) );
          fprintf(fa,"%f ",get_anns_avg(j));
          fprintf(fs,"%f ",get_anns_std(j));
      }
      printf("\n");
      fprintf(fa,"\n");
      fprintf(fs,"\n");
  }
  fclose(fi);
  fclose(fa);
  fclose(fs);
  return 0;
}
