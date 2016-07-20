#include <fann.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
//NOTE: this assumes that the training database is the same for all NN

unsigned int verbose=1;
unsigned int nnans;
unsigned int loaded_anns=0;
// array of pointers to store all of the anns
struct fann **anns=0;
struct fann_train_data *data_avg=0, *data_std=0;

//=============
// LOADING ANNS
//=============
int load_anns(int n, char *argv[]){
  unsigned int j;
  struct fann *ann;
  const char *annFile;

  nnans=n;
  if(loaded_anns==0){

    // Allocate memory for anns
    if (verbose) printf("Allocate memory for %d anns\n", nnans);
    anns = malloc(nnans * sizeof(struct fann*));

    // Load the network from the file
    for (n = 0; n < nnans; n++){
        annFile = argv[n];
        if (verbose) printf("Reading network file %s\n", annFile);
        anns[n] = fann_create_from_file(annFile);
        if (anns[n] == NULL){
          if (verbose)  printf("Invalid network file %s\n", annFile);
          return -1;
        }
    }

    // Allocate memory for data
    data_avg = fann_create_train(1, anns[0]->num_input, anns[0]->num_output);
    data_std = fann_create_train(1, anns[0]->num_input, anns[0]->num_output);
    loaded_anns=1;
  }

  //initialize memory
  for(j = 0; j < anns[0]->num_input; j++){
    data_avg->input[0][j]=0.;
    data_std->input[0][j]=0.;
  }
  for(j = 0; j < anns[0]->num_output; j++){
    data_avg->output[0][j]=0.;
    data_std->output[0][j]=0.;
  }

  return 0;
}

//=============
// RUNNING ANNS
//=============
int run_anns(){
  unsigned int n, j;
  fann_type *calc_out;

  //copy avg input data to std input data
  for(j = 0; j < anns[0]->num_input; j++){
    data_std->input[0][j]=data_avg->input[0][j];
  }

  //run
  for (n = 0; n < nnans; n++){
     fann_scale_input( anns[n], data_avg->input[0] );
     calc_out = fann_run( anns[n], data_avg->input[0] );
     fann_descale_input( anns[n], data_avg->input[0] );
     fann_descale_output( anns[n], calc_out);
     for(j = 0; j != data_avg->num_output; j++){
       data_avg->output[0][j] += calc_out[j];
       data_std->output[0][j] += calc_out[j] * calc_out[j];
     }
  }

  // calculate avg and std
  for(j = 0; j != data_avg->num_output; j++){
      //std
      data_std->output[0][j]=sqrt( (data_std->output[0][j]- (data_avg->output[0][j]*data_avg->output[0][j])/nnans)/nnans );
      //avg
      data_avg->output[0][j]=data_avg->output[0][j]/nnans;
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
  unsigned int j,k,n,num_data;

  struct fann *ann;
  const char *runFile = argv[argc-1];
  FILE *fp1, *fp2;

  //initialize arrays
  load_anns(argc-2,argv+1);

  // Read input data
  if (verbose)  printf("Reading data from file %s: ", runFile);
  fp1 = fopen(runFile, "r");
  fscanf(fp1, "%u\n", &num_data);
  if (verbose)  printf("%u runs %d inputs %d ouputs\n", num_data, anns[0]->num_input, anns[0]->num_output);
  for(j = 0; j < anns[0]->num_input; j++){
    fscanf(fp1, FANNSCANF " ", &data_avg->input[0][j]);
  }
  fclose(fp1);

  //run anns
  run_anns();

  // print and write
  fp1 = fopen("output.avg", "w");
  fp2 = fopen("output.std", "w");
  fprintf(fp1,"%u\n",num_data);
  fprintf(fp2,"%u\n",num_data);
  for(j = 0; j < data_avg->num_output; j++){
      if (verbose)  printf("%f (%f) ",data_avg->output[0][j],data_std->output[0][j] );
      fprintf(fp1,"%f ",data_avg->output[0][j]);
      fprintf(fp2,"%f ",data_std->output[0][j]);
  }
  if (verbose)  printf("\n");
  fprintf(fp1,"\n");
  fprintf(fp2,"\n");
  fclose(fp1);
  fclose(fp2);
  return 0;
}
