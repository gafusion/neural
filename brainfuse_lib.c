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
struct fann_train_data *data_avg, *data_std;

//=============
// LOADING ANNS
//=============
int load_anns(int n, char *argv[]){
  unsigned int j;
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

int load_anns_(int n, char *argv[]){
  return load_anns(n,argv);
}

int load_anns__(int n, char *argv[]){
  return load_anns(n,argv);
}

//=================
// LOAD ANNS INPUTS
//=================
int load_anns_inputs(fann_type *data_in){
  unsigned int j;
  if (verbose)  printf("Reading ANNs input data %d inputs %d ouputs\n", anns[0]->num_input, anns[0]->num_output);
  for(j = 0; j < anns[0]->num_input; j++){
    data_avg->input[0][j]=(fann_type)data_in[j];
    data_std->input[0][j]=data_avg->input[0][j];
  }
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
      data_std->output[0][j]=sqrt( (data_std->output[0][j] - (data_avg->output[0][j]*data_avg->output[0][j])/nnans)/nnans );
      //avg
      data_avg->output[0][j]=data_avg->output[0][j]/nnans;
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

fann_type * get_anns_avg(){
  return data_avg->output[0];
}

fann_type * get_anns_avg_(){
  return get_anns_avg();
}

fann_type * get_anns_avg__(){
  return get_anns_avg();
}

//--

fann_type * get_anns_std(){
  return data_std->output[0];
}

fann_type * get_anns_std_(){
  return get_anns_std();
}

fann_type * get_anns_std__(){
  return get_anns_std();
}