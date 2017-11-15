/* 多进程 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

int fork_new_process(int clifd, int (*func)(int clifd));
int handle_client(int clifd);

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
		}

		fork_new_process(clifd, handle_client);
	}
	
}

int fork_new_process(int clifd, int (*func)(int clifd))
{
	pid_t	pid;
	
	pid = fork();
	if(pid > 0)
		return 0;
	if(pid == 0)
	{
		printf("Create New Process\n");
		func(clifd);
		return 0;
	}

	printf("Parent Process exit\n");
	return 0;
}

int handle_client(int clifd)
{
	int		n;
	char	buffer[256];

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

	return 0;
}

int main(int args, char *argv[])
{
	int     sockfd;
    char    ch;
    char    *host = "192.168.1.46";
    int     port = 8001;

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