#include "http.h"

int handle_client(int connfd)
{
	char buffer[MAX_BUF_LEN];
	if(recv(connfd,buffer,MAX_BUF_LEN,0) < 0 )
		perror("recv() error..");

	buffer[MAX_BUF_LEN-1] = '\0';
	printf("HTTP-REQUEST: %s\n",buffer);

	if(send(connfd,"hallo",5,0) < 0)
		perror("send() error..");

	return 0;
}

