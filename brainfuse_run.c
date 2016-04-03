#include <fann.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

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
  const char *annFile;
  const char *runFile = argv[argc-1];
  struct fann_train_data *data_avg, *data_std, *data_lim, *data_nrm;
  fann_type *calc_out, *norm_output, tmp;
  FILE *fp1, *fp2, *fp3, *fp4;
  char dummy[100000];
  char *result = NULL;
  int verbose = 0;

  // Run
  for (n = 1; n < argc-1; n++){
      // Load the network from the file
      annFile = argv[n];
      if (verbose)  printf("Reading network file %s\n", annFile);
      ann = fann_create_from_file(annFile);
      if (ann == NULL){
        if (verbose)  printf("Invalid network file %s\n", annFile);
        return -1;
      }

    if (n==1){
        // Read input data
        if (verbose)  printf("Reading data from file %s: ", runFile);
        fp1 = fopen(runFile, "r");
        fscanf(fp1, "%u\n", &num_data);
        if (verbose)  printf("%u runs %d inputs %d ouputs\n", num_data, ann->num_input, ann->num_output);
        data_avg = fann_create_train(num_data, ann->num_input, ann->num_output);
        data_std = fann_create_train(num_data, ann->num_input, ann->num_output);
        data_lim = fann_create_train(num_data, ann->num_input, ann->num_output);
        data_nrm = fann_create_train(num_data, ann->num_input, ann->num_output);
    	for(i = 0; i < num_data; i++){
            for(j = 0; j < ann->num_input; j++){
                fscanf(fp1, FANNSCANF " ", &data_avg->input[i][j]);
			    data_std->input[i][j]=data_avg->input[i][j];
			    data_lim->input[i][j]=data_avg->input[i][j];
			    data_nrm->input[i][j]=data_avg->input[i][j];
		}
            for(j = 0; j < ann->num_output; j++){
			    data_avg->output[i][j]=0.;
			    data_std->output[i][j]=0.;
			    data_lim->output[i][j]=0.;
			    data_nrm->output[i][j]=1.;
			}
		}
	    fclose(fp1);

        //output normalization
        fp2 = fopen(annFile,"r");
        for(i = 0; i < 46; i++){
          fgets(dummy,100000,fp2);
        }
        if (fscanf(fp2,"norm_output=") == 1){
    	    for(i = 0; i < num_data; i++){
        	    for(j = 0; j < ann->num_output; j++){
                    for(k = 0; k < ann->num_input; k++){
                        fscanf(fp2, FANNSCANF " ", &tmp);
                        data_nrm->output[i][j]*=pow(data_nrm->input[i][k],tmp);
                    }
        	    }
        	}
        }
    	fclose(fp2);
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
          data_std->output[i][j]=sqrt( (data_std->output[i][j]- (data_avg->output[i][j]*data_avg->output[i][j])/(argc-2))/(argc-2) ) * data_nrm->output[i][j];
          //avg
          data_avg->output[i][j]=data_avg->output[i][j]/(argc-2) * data_nrm->output[i][j];
          //rng (NOTE: takes last ANN and not respective ANN, but it's good approx)
	      data_lim->output[i][j]=data_std->output[i][j]/ann->scale_deviation_out[j];
      }
      //lim (NOTE: takes last ANN and not respective ANN, but it's good approx)
      fann_scale_input( ann, data_lim->input[i] );
  }

  // print and write
  fp1 = fopen("output.avg", "w");
  fp2 = fopen("output.std", "w");
  fp3 = fopen("input.lim" , "w");
  fp4 = fopen("output.rng", "w");
  fprintf(fp1,"%u\n",num_data);
  fprintf(fp2,"%u\n",num_data);
  fprintf(fp3,"%u\n",num_data);
  fprintf(fp4,"%u\n",num_data);
  for(i = 0; i < data_avg->num_data; i++){
      if (verbose)  printf("Run %d: ",i);
      for(j = 0; j < data_avg->num_input; j++){
        fprintf(fp3,"%f ",data_lim->input[i][j]);
      }
      for(j = 0; j < data_avg->num_output; j++){
          if (verbose)  printf("%f (%f) ",data_avg->output[i][j],data_std->output[i][j] );
          fprintf(fp1,"%f ",data_avg->output[i][j]);
          fprintf(fp2,"%f ",data_std->output[i][j]);
          fprintf(fp4,"%f ",data_lim->output[i][j]);
      }
      if (verbose)  printf("\n");
      fprintf(fp3,"\n");
      fprintf(fp1,"\n");
      fprintf(fp2,"\n");
      fprintf(fp4,"\n");
  }
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
  return 0;
}
