
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h> /* memset */
#include <unistd.h>
#include <sys/wait.h>

#include "http.h"

/* listening port */
#define SERV_PORT 9980

/* maxium of allowed clients at the same time */
#define MAX_CLIENTS 10

/* counts our clients */
unsigned int clientcount;

/* maximum length for the queue of pending connections */
#define LISTENQ 1024

/*
   our child-handler. wait for all children to avoid zombies
 */
void sigchld_handler(int signo)
{
    int status;
    pid_t pid;

    /*
       -1 means that we wait until the first process is terminated
       WNOHANG tells the kernel not to block if there are no terminated
       child-processes.
     */
    while( (pid = waitpid(-1,&status,WNOHANG)) > 0)
    {
	clientcount--;
    	printf("%i exited with %i\n", pid, WEXITSTATUS(status));
    }

    return;
}


int main(int argc, char* argv[])
{
	int sockfd, connfd;
	pid_t	childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	struct sigaction sa; /* for wait-child-handler */

	clientcount = 0;

	/* Create Server-Socket of type IP( IP: 0) */
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket() error..");
		exit(EXIT_FAILURE);
	}

	/* Prepare our address-struct */
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* bind server to all interfaces */
	servaddr.sin_port = htons(SERV_PORT); /* bind server to SERV_PORT */

	/* bind sockt to address + port */
	if(bind(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr)) != 0)
	{
		perror("bind() failed..");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	/* start listening */
	if(listen(sockfd,LISTENQ) < 0)
	{
		perror("listen() error..");
		exit(EXIT_FAILURE);
	}

	
	memset(&sa, 0, sizeof(sa));
    	sa.sa_handler = sigchld_handler;
    	sigaction(SIGCHLD, &sa, NULL);
	

	/* endless listening-loop 
	   this loop waits for client-connections.
	   if a client is connectd it forks into another
	   subprocess.
	 */
	while(1)
	{
	    if(clientcount < MAX_CLIENTS)
	    {	
		clilen = sizeof(cliaddr);
		/* accept connections from clients */
		connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
		if(connfd < 0)
		{
			/*
			   withouth this we would recieve:
			    "accept() error..: Interrupted system call" after
			    each client disconnect. this happens because SIGCHLD
			    blocks our parent-accept(). 
			 */  
			if (errno == EINTR)
				continue;

			perror("accept() error..");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		clientcount++;

		/* lets create a subprocess */
		childpid = fork();
		if(childpid < 0)
		{
			perror("fork() failed");
			exit(EXIT_FAILURE);
		}

		/* let's start our child-subprocess */
		if( childpid  == 0 )
		{
			close(sockfd);
			exit(handle_client(connfd));
		}

		/* continue our server-routine */
		printf("Client has PID %i\n",childpid);

		close(connfd);
	   }	
	}

	return EXIT_SUCCESS;

}
	
