#include <fann.h>
int load_anns(char *string, char *basename);
int load_anns_inputs(fann_type *data_in);
int run_anns();
fann_type get_anns_avg(int j);
fann_type get_anns_std(int j);
int get_anns_avg_array(fann_type d);
int get_anns_std_array(fann_type d);
int get_anns_num_input();
int get_anns_num_output();
