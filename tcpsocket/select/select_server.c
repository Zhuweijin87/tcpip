/* select 网络服务 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define LISTEN_Q_MAX	20
#define MAX_CLIENTS		1024

int socket_listen(char *host, int port)
{
	int		ret, listenfd;
	struct sockaddr_in server;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		fprintf(stderr, "fail to socket: %s\n", strerror(errno));
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(host);
	
	ret = bind(listenfd, (struct sockaddr *)&server, sizeof(server));
	if(ret < 0)
	{
		fprintf(stderr, "fail to bind: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	ret = listen(listenfd, LISTEN_Q_MAX);
	if(ret < 0)
	{
		fprintf(stderr, "listen error: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	return listenfd;
}

int handle_select(int listenfd)
{
	int				maxfd;
	int				fdsets[MAX_CLIENTS];
	int				numOfCli;
	fd_set			readfds, allfds;
	struct timeval	tv;

	int i;
	for(i=0; i<MAX_CLIENTS; i++)
		fdsets[i] = -1;

	FD_ZERO(&allfds);
	FD_SET(listenfd, &allfds);
	maxfd = listenfd;
	
	for ( ; ; )
	{
		readfds = allfds;
		/* 设置扫描时间 */
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		
		numOfCli = select(maxfd+1, &readfds, NULL, NULL, &tv);
		if(numOfCli == -1)
		{
			fprintf(stderr, "select error: %s\n", strerror(errno));
			return -1;
		}
		
		if(numOfCli == 0)
		{
			continue;
		}

		if(FD_ISSET(listenfd, &readfds))
		{
			/* 处理客户端连接 */
			handle_accept(listenfd, &maxfd, fdsets, &allfds);
		}
		
		/* 处理数据读写 */
		handle_client(numOfCli, &maxfd, fdsets, &readfds, &allfds);
	}

	return 0;
}

/* 处理客户端连接 */
int handle_accept(int listenfd, int *maxfd, int *fdsets, fd_set *allfds)
{
	int		i, clifd;
	struct sockaddr_in client;
	socklen_t	clientlen;

	clientlen = sizeof(struct sockaddr_in);
	clifd = accept(listenfd, (struct sockaddr *)&client, &clientlen);
	if(clifd == -1)
	{
		return -1;
	}

	for(i=0; i<(*maxfd); i++)
	{
		if(fdsets[i] == -1)
		{
			fdsets[i] = clifd;
			FD_SET(clifd, allfds);
		}
	}

	printf("New Client Connect\n");

	(*maxfd) = (*maxfd) > clifd ? (*maxfd) : clifd;

	return 0;
}

int handle_client(int numOfCli, int *maxfd, int *fdsets, fd_set *readfds, fd_set *allfds)
{
	int 	len, i;
	char	buf[1028] = {'\0'};
	int		maxCount = *maxfd;

	for(i=0; i<maxCount; i++)
	{
		if(FD_ISSET(fdsets[i], readfds))
		{
			memset(buf, 0, sizeof(buf));
			len = read(fdsets[i], buf, 1024);
			if(len == -1)
			{
				fprintf(stderr, "recv error:%s\n", strerror(errno));
				close(fdsets[i]);
				FD_CLR(fdsets[i], allfds);
				fdsets[i] = -1;
			}
			else if(len == 0)
			{
				fprintf(stderr, "client[%d] closed\n", fdsets[i]);
				close(fdsets[i]);
				FD_CLR(fdsets[i], allfds);
				fdsets[i] = -1;
			}
			else
			{
				fprintf(stderr, "recv: %s\n", buf);
			}
		}
	}

	return 0;
}

int main(int args, char *argv[])
{
	char	*host = "192.168.1.46";
	int		 port = 8001;
	char	 ch;

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

    printf("Host:%s, Port:%d\n", host, port);

	int sockfd = socket_listen(host, port);	
	if(sockfd < 0)
		return -1;

	handle_select(sockfd);

	return 0;
}
