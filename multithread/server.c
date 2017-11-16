/* 多线程 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

void *handle_client(void *arg);
void call_thread_run(int clifd, void* (*func)(void *arg));

int socket_listen(char *host, int port)
{
	int		sockfd;
	int		ret;
	struct sockaddr_in server;
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		fprintf(stderr, "fail to socket: %s\n", strerror(errno));
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(host);

	ret = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1)
	{
		fprintf(stderr, "fail to bind: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	ret = listen(sockfd, 20);
	if(ret == -1)
	{
		fprintf(stderr, "fail to listen: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	return sockfd;
}

/* 处理连接 */
int handle_connect(int sockfd)
{
	int		clifd;
	struct sockaddr_in client;
	socklen_t	clilen;

	while(1)
	{
		clilen = sizeof(struct sockaddr);
		clifd = accept(sockfd, (struct sockaddr *)&client, &clilen);
		if(clifd == -1)
		{
			fprintf(stderr, "fail to accept: %s\n", strerror(errno));
			close(clifd);
			return -1;
		}
		printf("Accept New Client\n");
		call_thread_run(clifd, handle_client);
	}

	return 0;	
}

void call_thread_run(int clifd, void* (*func)(void *arg))
{
	int			ret;
	pthread_t	thread;

	pthread_create(&thread, NULL, handle_client, &clifd);
	
	//pthread_join(thread, NULL);
}

void *handle_client(void *arg)
{
	int 	clifd, n;
	char	buffer[256];
	clifd = *(int*)(arg);

	printf("Creat New Thread\n");
	while(1)
	{
		memset(buffer, 0, sizeof(buffer));
		n = read(clifd, buffer, 255);
		if(n == -1)
		{
			fprintf(stderr, "fail to read: %s\n", strerror(errno));
			break;
		}
		else if(n==0)
		{
			fprintf(stderr, "client disconnect\n");
			close(clifd);
			break;
		}
		else
		{
			fprintf(stderr, "RECV: %s\n", buffer);
		}
	}

	return (void *)-1;
}

int main(int args, char *argv[])
{
	int 	sockfd;
	char	ch;
	char	*host = "192.168.1.46";
	int 	port = 8001;

	while( (ch = getopt(args, argv, "s:p:")) != -1)
	{
		switch(ch)
		{
		case 's':
				host = optarg;
				break;
		case 'p':
				port = atoi(optarg);
				break;
		default:
				break;
		}
	}

	printf("Host: %s, Port:%d\n", host, port);

	sockfd = socket_listen(host, port);
	if(sockfd == -1)
		return -1;

	handle_connect(sockfd);

	return 0;	
}
