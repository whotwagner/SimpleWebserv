
/* Copyright (C) 2014 Wolfgang Hotwagner <code@feedyourhead.at>       
 *                                                                
 * This file is part of WebServ                                            
 *
 * concut is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version.
 *
 * concut is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License          
 * along with concut; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA 
 *
 */

#include "http.h"

int parse_first_http(const char* http_req, const int req_len)
{
	char method[MAX_METHOD_LEN];
	char path[MAX_PATH_LEN];
	char http[MAX_HTTPSTR_LEN];
	int i = 0;
	int j = 0;
	
	if(http_req == NULL)
		return -1;

	printf("REQUEST-LEN: %d\n",req_len); 

	/* parse method-string */
	while( http_req[i] != ' ' )
	{
		if(http_req[i] == NULL)
			return -2;

		if(j >= MAX_METHOD_LEN-1)
			return -3;

		if(i == req_len)
			return -4;

		method[j++] = http_req[i++];
	}

	method[j] = '\0';

	printf("METHOD: %s\n",method);
	/* At the moment only GET is supported */
	if(strcmp(method,"GET") != 0)
		return -11;
	j = 0;
	i++;

	/* parse path-string */
	while( http_req[i] != ' ' && (http_req[i] != NULL)) 
	{
		if(j >= MAX_PATH_LEN-1)
			return -5;

		if(i == req_len)
			return -6;

		path[j++] = http_req[i++];
	}

	path[j] = '\0';

	printf("PATH: %s\n",path);

	/* it's a simple request */
	if(i == req_len)
		return 0;

	i++;
	j = 0;

	/* parse http-string */
	while( http_req[i] != ' ' && http_req[i] != '\r' && (http_req[i] != NULL)) 	
	{
		if(j >= MAX_HTTPSTR_LEN-1)
			return -7;

		if(i == req_len)
			return -8;

		http[j++] = http_req[i++];
	}

	http[j] = '\0';
	printf("HTTP-STR: %s\n",http);
	if((strcmp(http,"HTTP/1.0") != 0) && (strcmp(http,"HTTP/1.1") != 0))
		return -12;

	return 1;
}

int handle_client(int connfd)
{
	char buffer[MAX_BUF_LEN+1];
	int n = 0;
	int req_type = 0;
	const char sendok[1024] = "HTTP/1.0 200 OK\r\nServer: SimpleWebserver\r\nContent-Type: text/html\r\n\n";
	
	/* recieve the http-request */
	n = recv(connfd,buffer,MAX_BUF_LEN,0);
	if(n <= 0)
	{
		perror("recv() error..");
		close(connfd);
		return 0;
	}
	buffer[n] = '\0';
	
	printf("HTTP-REQUEST: ..%s..\n",buffer);

	req_type = parse_first_http(buffer,n);
	if(req_type == 0)
	{
		printf("it's a simple request\n");
		send(connfd,sendok,strlen(sendok),0);
		printf("HTTP-OK SENT\n");
	}
	else if(req_type == 1)
	{
		printf("it's a complex multiline request\n");
		send(connfd,sendok,strlen(sendok),0);
		printf("HTTP-OK SENT\n");
	}
	else
	{
		fprintf(stderr,"parse_first_http() error: %d\n",req_type);
		close(connfd);
		return -1;
	}


	char sendbuf[1024] = "<html>\n<head><title>TESTSITE</title></head>\n<body>This is just a test<cr>\n</body>\n</html>\n";
	if(send(connfd,sendbuf,strlen(sendbuf),0) < 0)
		perror("send() error..");

	close(connfd);
	return 0;
}

