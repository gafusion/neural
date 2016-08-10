#include <fann.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

static unsigned int n_models=2; //number of nn physics models
static unsigned int verbose=0;

// arrays of pointers storing multiple ANNS instances,
// of multiple ANNS ensembles, for different physics models
unsigned int *nanns=NULL;
unsigned int *loaded_anns=NULL;
struct fann ***anns;
struct fann_train_data **data_avg, **data_std, **data_nrm;
int model;
fann_type ****nrm_table;

//=============
// LOADING ANNS
//=============

int load_anns(int global_nn_model, char *directory, char *basename){
  DIR *dir;
  int n,k,j;
  struct dirent *ent;
  char annFile[2000];
  char dummy[100000];
  char * pch;
  FILE *fp2;
  fann_type tmp;

  model=global_nn_model;

  if (nanns == NULL){
    nanns = calloc(n_models, sizeof(unsigned int));
    loaded_anns = calloc(n_models, sizeof(unsigned int));
    anns = malloc(n_models * sizeof(struct fann**));
    data_avg = malloc(n_models * sizeof(struct fann_train_data *));
    data_std = malloc(n_models * sizeof(struct fann_train_data *));
    data_nrm = malloc(n_models * sizeof(struct fann_train_data *));
    nrm_table = malloc(n_models * sizeof(fann_type***));
  }

  if(verbose) printf("NN model %d\n",model);

  if(loaded_anns[model]!=0){
    if(verbose) printf("NN files already loaded\n");

  }else{

    if ((dir = opendir(directory)) == NULL) {
      printf("could not open directory: %s",directory);
      return -1;
    }

    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name,basename,9)==0){
        nanns[model]+=1;
      }
    }
    closedir (dir);
    if (nanns[model]==0){
      return 0;
    }

    // Allocate memory for anns
    if (verbose) printf("Allocate memory for %d anns\n", nanns[model]);
    anns[model] = malloc(nanns[model] * sizeof(struct fann*));

    // Load the network from the file
    n=0;
    dir = opendir(directory);
    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name,basename,9)==0){
        sprintf(annFile, "%s/%s", directory,ent->d_name);
        anns[model][n] = fann_create_from_file(annFile);
        if (anns[model][n] == NULL){
          printf("Invalid network file %s\n", annFile);
          return -1;
        }
        else if (verbose) printf("%d Reading network file %s\n",n, annFile);
        n+=1;
      }
    }
    closedir (dir);

    // Allocate memory for data
    data_avg[model] = fann_create_train(1, anns[model][0]->num_input, anns[model][0]->num_output);
    data_std[model] = fann_create_train(1, anns[model][0]->num_input, anns[model][0]->num_output);
    data_nrm[model] = fann_create_train(1, anns[model][0]->num_input, anns[model][0]->num_output);
    nrm_table[model] = malloc(nanns[model] * sizeof(fann_type**));
    for(n=0; n<nanns[model]; n++){
        nrm_table[model][n] = malloc(anns[model][0]->num_input * sizeof(fann_type*));
        for(k=0; k<anns[model][0]->num_input; k++){
            nrm_table[model][n][k] = malloc(anns[model][0]->num_output * sizeof(fann_type));
        }
    }

    // Power-law outputs normalization
    n=0;
    dir = opendir(directory);
    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name,basename,9)==0){
        sprintf(annFile, "%s/%s", directory,ent->d_name);
        fp2 = fopen(annFile,"r");
        for(k = 0; k < 47; k++){
          fgets(dummy,100000,fp2);
        }
        if (strstr(dummy, "norm_output=") != NULL){
            pch = strtok(dummy+12," \n");
            for(j = 0; j < anns[model][0]->num_output; j++){
                for(k = 0; k < anns[model][0]->num_input; k++){
                   nrm_table[model][n][k][j]=(fann_type)atof(pch);
                   pch = strtok (NULL, " \n");
                }
            }
        }
        fclose(fp2);
        if (verbose) printf("%d Setting normalization %s\n",n, annFile);
        n+=1;
      }
    }
    closedir (dir);

    // Set loaded flag
    loaded_anns[model]=1;
  }

  // Initialize memory
  for(j = 0; j < anns[model][0]->num_input; j++){
    data_avg[model]->input[0][j]=0.;
    data_std[model]->input[0][j]=0.;
  }
  for(j = 0; j < anns[model][0]->num_output; j++){
    data_avg[model]->output[0][j]=0.;
    data_std[model]->output[0][j]=0.;
  }

  return n;
}

int load_anns_(int *global_nn_model, char *directory, char *basename){
  return load_anns(*global_nn_model, directory, basename);
}

int load_anns__(int *global_nn_model, char *directory, char *basename){
  return load_anns(*global_nn_model, directory, basename);
}

//=================
// LOAD ANNS INPUTS
//=================
int load_anns_inputs(fann_type *data_in){
  unsigned int j;
  if (verbose)  printf("Reading ANNs input data %d inputs %d ouputs\n", anns[model][0]->num_input, anns[model][0]->num_output);
  for(j = 0; j < anns[model][0]->num_input; j++){
    if (verbose) printf("in  %02d: %3.3f\n",j+1,data_in[j]);
    data_avg[model]->input[0][j]=(fann_type)data_in[j];
    data_std[model]->input[0][j]=data_avg[model]->input[0][j];
    data_nrm[model]->input[0][j]=data_avg[model]->input[0][j];
  }
  if (verbose) printf("\n");
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
  unsigned int n,k,j;
  fann_type *calc_out;

  if (verbose) printf("Running ANNs\n");

  // run
  for (n = 0; n < nanns[model]; n++){

     // power law normalization
     for(j = 0; j < anns[model][0]->num_output; j++){
        data_nrm[model]->output[0][j]=1.;
        for(k = 0; k < anns[model][0]->num_input; k++){
            if (nrm_table[model][n][k][j]!=0.0){
                data_nrm[model]->output[0][j]*=pow(data_nrm[model]->input[0][k],nrm_table[model][n][k][j]);
             }
        }
     }

     // scale - run - descale
     fann_scale_input( anns[model][n], data_avg[model]->input[0] );
     calc_out = fann_run( anns[model][n], data_avg[model]->input[0] );
     fann_descale_input( anns[model][n], data_avg[model]->input[0] );
     fann_descale_output( anns[model][n], calc_out);

     // avg and std (part 1)
     for(j = 0; j != data_avg[model]->num_output; j++){
       data_avg[model]->output[0][j] += calc_out[j] * data_nrm[model]->output[0][j];
       data_std[model]->output[0][j] += calc_out[j] * data_nrm[model]->output[0][j] * calc_out[j] * data_nrm[model]->output[0][j];
     }
  }

  // calculate avg and std (part 2)
  for(j = 0; j != data_avg[model]->num_output; j++){
      // std
      data_std[model]->output[0][j]=sqrt( (data_std[model]->output[0][j] - (data_avg[model]->output[0][j]*data_avg[model]->output[0][j])/nanns[model])/nanns[model] );
      // avg
      data_avg[model]->output[0][j]=data_avg[model]->output[0][j]/nanns[model];
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
  return anns[model][0]->num_output;
}

int get_anns_num_output_(){
  return get_anns_num_output();
}

int get_anns_num_output__(){
  return get_anns_num_output();
}

//--

int get_anns_num_input(){
  return anns[model][0]->num_input;
}

int get_anns_num_input_(){
  return get_anns_num_input();
}

int get_anns_num_input__(){
  return get_anns_num_input();
}

//--

fann_type get_anns_avg(int j){
  return data_avg[model]->output[0][j];
}

int get_anns_avg_array(fann_type* d){
  int j;
  for(j = 0; j != data_avg[model]->num_output; j++){
    if (verbose) printf("avg %02d: %3.3f\n",j+1,data_avg[model]->output[0][j]);
    d[j]=data_avg[model]->output[0][j];
  }
  return 0;
}

int get_anns_avg_array_(fann_type* d){
  return get_anns_avg_array(d);
}

int get_anns_avg_array__(fann_type* d){
  return get_anns_avg_array(d);
}

//--

fann_type get_anns_std(int j){
  return data_std[model]->output[0][j];
}

int get_anns_std_array(fann_type* d){
  int j;
  for(j = 0; j != data_std[model]->num_output; j++){
    if (verbose) printf("std %02d: %3.3f\n",j+1,data_std[model]->output[0][j]);
    d[j]=data_std[model]->output[0][j];
  }
  return 0;
}

int get_anns_std_array_(fann_type* d){
  return get_anns_std_array(d);
}

int get_anns_std_array__(fann_type* d){
  return get_anns_std_array(d);
}
