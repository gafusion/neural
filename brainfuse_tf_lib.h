int hostname_to_ip(char * hostname , char* ip);
int set_btf_verbose(int verbose);
int set_btf_model(char *model);
int set_btf_host(char *host);
int set_btf_port(int port);
int init_btf(char *table);
int btf_run(double *input, int input_len, double *output, int output_len);
