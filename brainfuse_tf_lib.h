int hostname_to_ip(char * hostname , char* ip);
int set_btf_verbose(int verbose);
int set_btf_payload_flt_array(char *btf_sendline, float *data, int len);
int set_btf_payload_dbl_array(char *btf_sendline, double *data, int len);
int set_btf_host(char *host);
int set_btf_port(int port);
int init_btf(char *table, char *btf_sendline, int n);
int btf_send(char* btf_sendline);
