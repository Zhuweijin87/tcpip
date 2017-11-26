#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int start_listen(char *host, int port)
{
    int     ret;
    int     sockfd;
    struct sockaddr_in	server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		fprintf(stderr, "fail to socket: %s\n", strerror(errno));
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(host);

	ret = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1)
	{
		fprintf(stderr, "fail to bind: %s\n", strerror(errno));
		exit(1);
	}

	ret = listen(sockfd, 20);
	if(ret == -1)
	{
		fprintf(stderr, "fail to listen: %s\n", strerror(errno));
		exit(1);
	}

    return sockfd;
}

int handle_connection(int fd)
{
    return 0;
}

int server_run(int sockfd)
{   
    int     connfd;
    struct  sockaddr_in  client;
    socklen_t  len = sizeof(client);
    while(1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);
        if(connfd < 0)
        {
            fprintf(stderr, "");
        }

        handle_connection(connfd);
    }
    return 0;
}

int main(int args, char *argv)
{
    int		ret;
	int 	sockfd;
	char	ch;
	char	*host = "192.168.1.46";
	int		port = 8010;

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

    sockfd = start_listen(host, port);


}