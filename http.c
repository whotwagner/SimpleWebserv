#include "http.h"

int handle_client(int connfd)
{
	char buffer[1024];
	if(recv(connfd,buffer,1024,0) < 0 )
		perror("recv() error..");

	if(send(connfd,"hallo",5,0) < 0)
		perror("send() error..");

	return 0;
}

