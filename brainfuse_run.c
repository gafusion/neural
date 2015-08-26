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
  unsigned int i,j,n,num_data;
  struct fann *ann;
  const char *annFile;
  const char *runFile = argv[argc-1];
  struct fann_train_data *data_avg, *data_std, *data_lim;
  fann_type *calc_out;
  FILE *fp1, *fp2, *fp3;

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

    if (n==1){
        // Read input data
        printf("Reading data from file %s: ", runFile);
        fp1 = fopen(runFile, "r");
        fscanf(fp1, "%u\n", &num_data);
        printf("%u runs %d inputs %d ouputs\n", num_data, ann->num_input, ann->num_output);
        data_avg = fann_create_train(num_data, ann->num_input, ann->num_output);
        data_std = fann_create_train(num_data, ann->num_input, ann->num_output);
        data_lim = fann_create_train(num_data, ann->num_input, ann->num_output);
    	for(i = 0; i < num_data; i++){
		    for(j = 0; j < ann->num_input; j++){
			    fscanf(fp1, FANNSCANF " ", &data_avg->input[i][j]);
			    data_std->input[i][j]=data_avg->input[i][j];
			    data_lim->input[i][j]=data_avg->input[i][j];
			}
            for(j = 0; j < ann->num_output; j++){
			    data_avg->output[i][j]=0.;
			    data_std->output[i][j]=0.;
			    data_lim->output[i][j]=0.;
			}
		}
	    fclose(fp1);
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
          //lim
          data_lim->output[i][j]=data_avg->output[i][j];
      }
      //lim
      fann_scale_input( ann, data_lim->input[i] );
      fann_scale_output( ann, data_lim->output[i] );
  }

  // print and write
  fp1 = fopen("output.avg", "w");
  fp2 = fopen("output.std", "w");
  fp3 = fopen("input.lim", "w");
  fprintf(fp1,"%u\n",num_data);
  fprintf(fp2,"%u\n",num_data);
  fprintf(fp3,"%u\n",num_data);
  for(i = 0; i < data_avg->num_data; i++){
      printf("Run %d: ",i);
      for(j = 0; j < data_avg->num_input; j++){
        fprintf(fp3,"%f ",data_lim->input[i][j]);
      }
      fprintf(fp3,"\n");
      for(j = 0; j < data_avg->num_output; j++){
          printf("%f (%f) ",data_avg->output[i][j],data_std->output[i][j]);
          fprintf(fp1,"%f ",data_avg->output[i][j]);
          fprintf(fp2,"%f ",data_std->output[i][j]);
          //fprintf(fp3,"%f ",data_lim->output[i][j]);
      }
      printf("\n");
      fprintf(fp1,"\n");
      fprintf(fp2,"\n");
      //fprintf(fp3,"\n");
  }

  fclose(fp1);
  fclose(fp2);
  return 0;
}
