#include <fann.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
//NOTE: this assumes that the training database is the same for all NN

unsigned int verbose=0;
unsigned int nanns;
unsigned int loaded_anns=0;
// array of pointers to store all of the anns
struct fann **anns=0;
struct fann_train_data *data_avg, *data_std;

//=============
// LOADING ANNS
//=============

int load_anns(char *directory, char *basename){
  DIR *dir;
  int n,j;
  struct dirent *ent;
  char annFile[2000];

  if(loaded_anns==0){
    if ((dir = opendir(directory)) == NULL) {
      printf("could not open directory: %s",directory);
      return -1;
    }

    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name,basename,9)==0){
        nanns+=1;
        if(verbose) printf ("%d,%s\n", nanns, ent->d_name);
      }
    }
    closedir (dir);
    if (nanns==0){
      return 0;
    }

    // Allocate memory for anns
    if (verbose) printf("Allocate memory for %d anns\n", nanns);
    anns = malloc(nanns * sizeof(struct fann*));

    // Load the network from the file
    n=0;
    dir = opendir(directory);
    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name,basename,9)==0){
        sprintf(annFile, "%s/%s", directory,ent->d_name);
        anns[n] = fann_create_from_file(annFile);
        if (anns[n] == NULL){
          printf("Invalid network file %s\n", annFile);
          return -1;
        }
        else if (verbose) printf("%d Reading network file %s\n",n, annFile);
        n+=1;
      }
    }
    closedir (dir);

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

  return n;
}

int load_anns_(char *directory, char *basename){
  return load_anns(directory, basename);
}

int load_anns__(char *directory, char *basename){
  return load_anns(directory, basename);
}

//=================
// LOAD ANNS INPUTS
//=================
int load_anns_inputs(fann_type *data_in){
  unsigned int j;
  if (verbose)  printf("Reading ANNs input data %d inputs %d ouputs\n", anns[0]->num_input, anns[0]->num_output);
  for(j = 0; j < anns[0]->num_input; j++){
    printf("%f ",data_in[j]);
    data_avg->input[0][j]=(fann_type)data_in[j];
    data_std->input[0][j]=data_avg->input[0][j];
  }
  printf("\n");
  return 0;
}

int load_anns_inputs_(fann_type *data_in){
  return load_anns_inputs(data_in);
}

int load_anns_inputs__(fann_type *data_in){
  return load_anns_inputs(data_in);
}

//=============
// RUNNING ANNS
//=============
int run_anns(){
  unsigned int n, j;
  fann_type *calc_out;

  if (verbose)  printf("Running ANNs\n");

  //run
  for (n = 0; n < nanns; n++){
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
      data_std->output[0][j]=sqrt( (data_std->output[0][j] - (data_avg->output[0][j]*data_avg->output[0][j])/nanns)/nanns );
      //avg
      data_avg->output[0][j]=data_avg->output[0][j]/nanns;
  }

  return 0;
}

int run_anns_(){
  return run_anns();
}

int run_anns__(){
  return run_anns();
}

//=============
// GET ANNS PROPERTIES and RESULTS
//=============
int get_anns_num_output(){
  return anns[0]->num_output;
}

int get_anns_num_output_(){
  return get_anns_num_output();
}

int get_anns_num_output__(){
  return get_anns_num_output();
}

//--

int get_anns_num_input(){
  return anns[0]->num_input;
}

int get_anns_num_input_(){
  return get_anns_num_input();
}

int get_anns_num_input__(){
  return get_anns_num_input();
}

//--

double get_anns_avg(int j){
  return (double)data_avg->output[0][j];
}

double get_anns_avg_(int *j){
  return (double)get_anns_avg(*j);
}

double get_anns_avg__(int *j){
  return (double)get_anns_avg(*j);
}

//--

double get_anns_std(int j){
  return (double)data_std->output[0][j];
}

double get_anns_std_(int *j){
  return (double)get_anns_std(j);
}

double get_anns_std__(int *j){
  return (double)get_anns_std(j);
}
