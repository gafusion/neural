#include <fann.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
//NOTE: this assumes that the training database is the same for all NN

int verbose=1;
const char *annFile;
// array of pointers to store all of the anns
struct fann **anns;
struct fann_train_data *data_avg, *data_std;

//=============
// LOADING ANNS
//=============
int load_anns(int nnets, char *argv[]){
  unsigned int n, i, j;
  struct fann *ann;

  anns = malloc(nnets * sizeof(struct fann*));

  // Load the network from the file
  for (n = 0; n < nnets; n++){
      annFile = argv[n];
      if (verbose) printf("Reading network file %s\n", annFile);
      anns[n] = ann = fann_create_from_file(annFile);
      if (ann == NULL){
        if (verbose)  printf("Invalid network file %s\n", annFile);
        return -1;
      }
  }

  // Allocate memory
  data_avg = fann_create_train(1, ann->num_input, ann->num_output);
  data_std = fann_create_train(1, ann->num_input, ann->num_output);
  for(j = 0; j < ann->num_input; j++){
    data_avg->input[0][j]=0.;
    data_std->input[0][j]=0.;
  }
  for(j = 0; j < ann->num_output; j++){
    data_avg->output[0][j]=0.;
    data_std->output[0][j]=0.;
  }

  return 0;
}

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
  unsigned int i,j,k,n,num_data;

  struct fann *ann;
  const char *runFile = argv[argc-1];
  fann_type *calc_out, *norm_output, tmp;
  FILE *fp1, *fp2, *fp3, *fp4;
  char dummy[100000];
  char *result = NULL;
  char * pch;

  load_anns(argc-2,argv+1);

  // Run
  for (n = 0; n < argc-2; n++){

    if (n==0){
        // Read input data and initialize arrays
        if (verbose)  printf("Reading data from file %s: ", runFile);
        fp1 = fopen(runFile, "r");
        fscanf(fp1, "%u\n", &num_data);
        if (verbose)  printf("%u runs %d inputs %d ouputs\n", num_data, anns[n]->num_input, anns[n]->num_output);
        for(j = 0; j < anns[n]->num_input; j++){
          fscanf(fp1, FANNSCANF " ", &data_avg->input[0][j]);
          data_std->input[0][j]=data_avg->input[0][j];
        }
        fclose(fp1);
    }

     //run
     fann_scale_input( anns[n], data_avg->input[0] );
     calc_out = fann_run( anns[n], data_avg->input[0] );
     fann_descale_input( anns[n], data_avg->input[0] );
     fann_descale_output( anns[n], calc_out);
     for(j = 0; j != data_avg->num_output; j++){
       data_avg->output[0][j] += calc_out[j];
       data_std->output[0][j] += calc_out[j] * calc_out[j];
     }
  }

  // avg and std
  for(j = 0; j != data_avg->num_output; j++){
      //std
      data_std->output[i][j]=sqrt( (data_std->output[i][j]- (data_avg->output[i][j]*data_avg->output[i][j])/(argc-2))/(argc-2) );
      //avg
      data_avg->output[i][j]=data_avg->output[i][j]/(argc-2);
  }

  // print and write
  fp1 = fopen("output.avg", "w");
  fp2 = fopen("output.std", "w");
  fprintf(fp1,"%u\n",num_data);
  fprintf(fp2,"%u\n",num_data);
  for(i = 0; i < data_avg->num_data; i++){
      if (verbose)  printf("Run %d: ",i);
      for(j = 0; j < data_avg->num_output; j++){
          if (verbose)  printf("%f (%f) ",data_avg->output[i][j],data_std->output[i][j] );
          fprintf(fp1,"%f ",data_avg->output[i][j]);
          fprintf(fp2,"%f ",data_std->output[i][j]);
      }
      if (verbose)  printf("\n");
      fprintf(fp1,"\n");
      fprintf(fp2,"\n");
  }
  fclose(fp1);
  fclose(fp2);
  return 0;
}
