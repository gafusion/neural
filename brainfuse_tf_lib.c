#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
char btf_model[256];
char *pToken[100];
char btf_host[100];
char btf_ip[15];
int  btf_port=8882;
int  btf_verbose=0;
int  btf_sendline_n=65507;

//Get ip from domain name
int hostname_to_ip(char * hostname , char* ip){
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    //strcpy(ip , "127.0.0.1" );
    //return 0;

    if ( (he = gethostbyname( hostname ) ) == NULL){
        // get the host info
        strcpy(ip , "127.0.0.1" );
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++){
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}

//verbosity
int set_btf_verbose(int verbose){
  btf_verbose=verbose;
  return 0;
}

int set_btf_verbose_(int *verbose){
  int verbose_ = *verbose;
  set_btf_verbose(verbose_);
  return 0;
}

int set_btf_verbose__(int *verbose){
  int verbose_ = *verbose;
  set_btf_verbose(verbose_);
  return 0;
}

//host
int set_btf_host(char *host){
  if (btf_verbose){
    printf("btf HOST: %s\n",host);
  }
  sprintf(btf_host,"%s",host);
  return 0;
}

int set_btf_host_(char *host){
  set_btf_host(host);
  return 0;
}

int set_btf_host__(char *host){
  set_btf_host(host);
  return 0;
}

//port
int set_btf_port(int port){
  btf_port=port;
  return 0;
}

int set_btf_port_(int *port){
  int port_ = *port;
  set_btf_port(port_);
  return 0;
}

int set_btf_port__(int *port){
  int port_ = *port;
  set_btf_port(port_);
  return 0;
}

//model
int set_btf_model(char *model){
  if (btf_verbose){
    printf("BTF MODEL: %s\n",model);
  }
  sprintf(btf_model,"%s",model);
  return 0;
}

int set_btf_model_(char *model){
  set_btf_model(model);
  return 0;
}

int set_btf_model__(char *model){
  set_btf_model(model);
  return 0;
}

//init
int init_btf(char *model){
  if (getenv("BTF_VERBOSE")!=NULL)
    set_btf_verbose(atoi(getenv("BTF_VERBOSE")));
  else
    set_btf_verbose(0);

  if (getenv("BTF_HOST")!=NULL){
    set_btf_host(getenv("BTF_HOST"));
  }else{
    set_btf_host("localhost");
    //set_btf_host("gadb-harvest.ddns.net");
  }
  hostname_to_ip(btf_host, btf_ip);

  if (getenv("BTF_PORT")!=NULL){
    set_btf_port(atoi(getenv("BTF_PORT")));
  }else{
      set_btf_port(8882);
  }

  if (getenv("BTF_MODEL")!=NULL && model[0]=='\0')
    set_btf_model(getenv("BTF_MODEL"));
  else if (model[0]!='\0')
    set_btf_model(model);

  return 0;
}

int init_btf_(char *model){
  init_btf(model);
  return 0;
}

int init_btf__(char *model){
  init_btf(model);
  return 0;
}

// This assumes buffer is at least x bytes long,
// and that the socket is blocking.
void ReadXBytes(int socket, unsigned int x, void* buffer){
    int bytesRead = 0;
    int result;
    while (bytesRead < x){
        result = read(socket, buffer + bytesRead, x - bytesRead);
        if (result < 1 ){
            printf("error on read\n");
            break;
        }
        bytesRead += result;
    }
}

void WriteXBytes(int socket, unsigned int x, void* buffer){
    int bytesWrite = 0;
    int result;
    while (bytesWrite < x) {
        result = write(socket, buffer + bytesWrite, x - bytesWrite);
        if (result < 1 ){
            printf("error on write\n");
            break;
        }
        bytesWrite += result;
    }
}

int parse_string(char pInputString[btf_sendline_n],char *Delimiter, char **pToken){
  int i=0;
  pToken[i] = strtok(pInputString, Delimiter);
  i++;
  while ((pToken[i] = strtok(NULL, Delimiter)) != NULL){
     i++;
  }
  return i;
}

//send
int btf_run(double *input, int input_len, double *output, int output_len){
  int sockfd;
  int i,n;
  struct sockaddr_in servaddr,cliaddr;
  char message1[btf_sendline_n],message2[btf_sendline_n];
  unsigned int length = 0;

  if (btf_model[0]=='\0'){
    perror("BTF_MODEL not set");
    return -1;
  }

  btf_verbose=1;

  srand(time(NULL));

  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=inet_addr(btf_ip);
  servaddr.sin_port=htons(btf_port);
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) <0){
    perror("ERROR connecting");
    return -1;
  }

  //compose request message
  sprintf(message1,"%s::(1,%d)::[",btf_model,input_len);
  for(i = 0; i < input_len-1; i++){
    sprintf(message1,"%s%g,",message1,*(input+i));
  }
  sprintf(message1,"%s%g]",message1,*(input+input_len-1));

  //send request
  length=strlen(message1);
  if (btf_verbose)
     printf("%s:%d >------> %s\n",btf_ip,btf_port,message1);
  WriteXBytes(sockfd, sizeof(length), (void*)(&length));
  WriteXBytes(sockfd, length, (void*)message1);

  //receive answer
  length=0;
  memset(message1+sizeof(length), 0, btf_sendline_n);
  ReadXBytes(sockfd, sizeof(length), (void*)(&length));
  ReadXBytes(sockfd, length, (void*)message1);
  if (btf_verbose)
     printf("%s:%d <------< %s\n",btf_ip,btf_port,message1);

  //parse answer message
  parse_string(message1,"::[",pToken);
  memset(message2, 0, btf_sendline_n);
  snprintf(message2,strlen(pToken[3]),",%s",pToken[3]+1);
  n=parse_string(message2,",",pToken);
  for(i = 0; i<n; i++){
    output[i]=atof(pToken[i]);
  }

  close(sockfd);
  return 0;
}

int btf_run_(double *input, int input_len, double *output, int output_len){
  return btf_run(input,input_len,output,output_len);
}

int btf_run__(double *input, int input_len, double *output, int output_len){
  return btf_run(input,input_len,output,output_len);
}
