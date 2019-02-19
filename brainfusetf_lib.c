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
int  btf_sendline_n=65507;

//Get ip from domain name
int btf_hostname_to_ip(char * hostname , char* ip){
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

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

// This assumes buffer is at least x bytes long,
// and that the socket is blocking.
int ReadXBytes(int socket, unsigned int x, void* buffer){
    int bytesRead = 0;
    int result;
    while (bytesRead < x){
        result = read(socket, buffer + bytesRead, x - bytesRead);
        if (result < 1 ){
            printf("error on read\n");
            return -1;
        }
        bytesRead += result;
    }
    return 0;
}

int WriteXBytes(int socket, unsigned int x, void* buffer){
    int bytesWrite = 0;
    int result;
    while (bytesWrite < x) {
        result = write(socket, buffer + bytesWrite, x - bytesWrite);
        if (result < 1 ){
            printf("error on write\n");
            return -1;
        }
        bytesWrite += result;
    }
    return 0;
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
int btf_run(char *model, double *input, int input_len, double *output, int output_len){
  int sockfd;
  int i,n;
  int ack;
  struct sockaddr_in servaddr,cliaddr;
  char message1[btf_sendline_n],message2[btf_sendline_n];
  unsigned int length = 0;
  char *pToken[100];
  char btf_host[100];
  char btf_ip[15];
  int  btf_port=8883;
  int  btf_verbose=0;
  int  btf_initialized=0;

  if (model[0]=='\0'){
    perror("BTF_MODEL not set");
    return -1;
  }

  if (btf_initialized!=1){
      if (getenv("BTF_VERBOSE")!=NULL)
        btf_verbose=atoi(getenv("BTF_VERBOSE"));

      if (getenv("BTF_HOST")!=NULL){
        sprintf(btf_host,"%s",getenv("BTF_HOST"));
      }else{
        sprintf(btf_host,"gadb-harvest.ddns.net");
      }
      btf_hostname_to_ip(btf_host, btf_ip);

      if (getenv("BTF_PORT")!=NULL){
        btf_port=atoi(getenv("BTF_PORT"));
      }

      btf_initialized=1;
  }

  srand(time(NULL));

  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=inet_addr(btf_ip);
  servaddr.sin_port=htons(btf_port);
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) <0){
    printf("HOST:%s  PORT:%d\n",btf_host,btf_port);
    perror("ERROR connecting");
    return -1;
  }

  //compose request message
  sprintf(message1,"%s&(1,%d)&[",model,input_len);
  for(i = 0; i < input_len-1; i++){
    sprintf(message1,"%s%g,",message1,*(input+i));
  }
  sprintf(message1,"%s%g]",message1,*(input+input_len-1));

  //send request
  for(i = 0; i < 10; i++){
      ack=0;

      length=strlen(message1);
      if (btf_verbose)
         printf("%s:%d >>>>>>> %s\n",btf_ip,btf_port,message1);
      ack+=WriteXBytes(sockfd, sizeof(length), (void*)(&length));
      ack+=WriteXBytes(sockfd, length, (void*)message1);

      //receive answer
      length=0;
      memset(message1+sizeof(length), 0, btf_sendline_n);
      ack+=ReadXBytes(sockfd, sizeof(length), (void*)(&length));
      ack+=ReadXBytes(sockfd, length, (void*)message1);
      if (btf_verbose)
         printf("%s:%d <<<<<<< %s\n",btf_ip,btf_port,message1);

      if (ack==0)
        break;
      usleep(10*i);
  }

  //parse answer message
  parse_string(message1,"&",pToken);
  memset(message2, 0, btf_sendline_n);
  snprintf(message2,strlen(pToken[2]),",%s",pToken[2]+1);
  n=parse_string(message2,",",pToken);
  for(i = 0; i<n; i++){
    output[i]=atof(pToken[i]);
  }

  close(sockfd);
  return 0;
}

int btf_run_(char *model, double *input, int *input_len, double *output, int *output_len){
  int input_len_ = *input_len;
  int output_len_ = *output_len;
  return btf_run(model, input, input_len_, output, output_len_);
}

int btf_run__(char *model, double *input, int *input_len, double *output, int *output_len){
  int input_len_ = *input_len;
  int output_len_ = *output_len;
  return btf_run(model, input, input_len_, output, output_len_);
}
