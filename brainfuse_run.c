#include <fann.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char *argv[])
{
  // Check input consistency
  if (argc < 3)
    {
      printf("Usage: %s ANN_file[s] run_file\n", argv[0]);
      return -1;
    }

  // Read in input parameters
  unsigned int i,j,n;
  struct fann* ann;
  const char* annFile;
  const char* runFile = argv[argc-1];
  struct fann_train_data *data_avg, *data_std;
  fann_type *calc_out;

  // Read data
  printf("Reading data from file %s\n", runFile);
  data_avg = fann_read_train_from_file(runFile);
  if(data_avg == NULL){
      printf("Invalid data file %s\n", runFile);
      return -1;
  }
  data_std = fann_create_train(data_avg->num_data, data_avg->num_input, data_avg->num_output);
  
  //zero
  for(i = 0; i != data_avg->num_data; i++){
    for(j = 0; j != data_avg->num_output; j++){
      data_avg->output[i][j]=0;
      data_std->output[i][j]=0;
    }
    for(j = 0; j != data_avg->num_input; j++){
        data_std->input[i][j]=data_avg->input[i][j];
    }
  }

  // Run
  for (n = 1; n < argc-1; n++){
      // Load the network from the file
      annFile = argv[n];
      printf("Reading network file %s\n", annFile);
      ann = fann_create_from_file(annFile);
      if (ann == NULL){
        return -1;
        printf("Invalid network file %s\n", annFile);
      }
      
     for(i = 0; i != data_avg->num_data; i++){
        fann_scale_input( ann, data_avg->input[i] );
        calc_out = fann_run( ann, data_avg->input[i] );
        fann_descale_input( ann, data_avg->input[i] );
        fann_descale_output( ann, calc_out);
        for(j = 0; j != data_avg->num_output; j++){
           data_avg->output[i][j]+=calc_out[j];
           data_std->output[i][j]+=calc_out[j]*calc_out[j];
        }
     }
  }

  // avg and std
  for(i = 0; i != data_avg->num_data; i++){
      for(j = 0; j != data_avg->num_output; j++){
          //std
          data_std->output[i][j]=sqrt( (data_std->output[i][j]- (data_avg->output[i][j]*data_avg->output[i][j])/(argc-2))/(argc-2) );
          //avg
          data_avg->output[i][j]=data_avg->output[i][j]/(argc-2);
      }
  }

  // print
  for(i = 0; i != data_avg->num_data; i++){
      printf("Run %d: ",i);
      for(j = 0; j != data_avg->num_output; j++){
          printf("%f (%f) ",data_avg->output[i][j],data_std->output[i][j]);
      }
      printf("\n");
  }

  // dump outputs
  fann_save_train(data_avg,"output_avg.dat");
  fann_save_train(data_std,"output_std.dat");

  return 0;
}
