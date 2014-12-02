#include "http.h"

#define MAX_METHOD_LEN 10
#define MAX_PATH_LEN 800

int parse_first_http(const char* http_req, const int req_len)
{
	const char *pbuf = http_req;
	char method[MAX_METHOD_LEN];
	char path[MAX_PATH_LEN];
	int i = 0;
	
	if(http_req == NULL)
		return -1;

	/* parse method-string */
	while( (*pbuf != NULL) && (*pbuf != ' ') && (i < MAX_METHOD_LEN-1) )
	{
		method[i] = *pbuf++;
		i++;
	}

	method[i] = '\0';

	printf("METHOD: %s\n",method);
	i = 0;
	*pbuf++;

	/* parse path-string */
	while( (*pbuf != NULL) && (*pbuf != ' ') && (i < MAX_PATH_LEN-1) )
	{
		path[i] = *pbuf++;
		i++;
	}

	path[i] = '\0';

	printf("PATH: %s\n",path);


	return 0;
}

int handle_client(int connfd)
{
	char buffer[MAX_BUF_LEN+1];
	int n = 0;
	
	n = recv(connfd,buffer,MAX_BUF_LEN,0);
	if(n <= 0)
	{
		perror("recv() error..");
		close(connfd);
		return 0;
	}

	buffer[n] = '\0';

	printf("FIRST-HTTP-REQUEST: %s\n",buffer);

	parse_first_http(buffer,n);

	while( (n = recv(connfd,buffer,MAX_BUF_LEN,0)) > 0)
	{
		buffer[n] = '\0';
		if( (strcmp(buffer,"\r\n") == 0) || (buffer[0] == '\n'))
			break;

		printf("HTTP-REQUEST: ..%s..\n",buffer);
	}

	if(send(connfd,"hallo",5,0) < 0)
		perror("send() error..");

	close(connfd);
	return 0;
}

