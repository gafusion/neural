int btf_hostname_to_ip(char * hostname , char* ip);
int set_btf_verbose(int verbose);
int set_btf_host(char *host);
int set_btf_port(int port);
int btf_run(char *model, double *input, int input_len, double *output, int output_len);
