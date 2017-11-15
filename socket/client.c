#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

int status = 1;

/* 捕捉SIGPIPE信号：当服务端断开连接时,客户端发送数据，会收到确认
 * 断开信息，再次发送数据就会抛出SIGPIPE信号 */
void sig_pipe(int signo)
{
	printf("Catch SIGPIPE signal\n");
	status = 0;
}

/* 在发送的时候，突然中断, SIG_INT*/
void sig_end(int signo)
{
	printf("Catch SIG_INT signal\n");
	exit(1);
}

int handle_with_write(int connfd)
{
	char		buffer[256] = {'\0'};

	while(status)
	{
		memset(buffer, 0, sizeof(buffer));
		printf(">:");
		fgets(buffer, 200, stdin);
		if(strncmp(buffer, "quit", 4) == 0)
			break;
		write(connfd, buffer, strlen(buffer)-1);
	}

	close(connfd);
	return 0;
}

int handle_with_send(int connfd)
{
	char		buffer[256] = {'\0'};

	while(status)
	{
		memset(buffer, 0, sizeof(buffer));
        printf(">:");
        fgets(buffer, 200, stdin);
        if(strncmp(buffer, "quit", 4) == 0)
            break;
		send(connfd, buffer, strlen(buffer)-1, 0);
	}

	close(connfd);
}

int main(int args, char *argv[])
{
	int		ret, connfd;
	char	ch;
	char	*host = "192.168.1.46";
	int		port = 8010;
	struct	sockaddr_in client;

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

	/* 信号处理 */
	signal(SIGPIPE, sig_pipe);
	signal(SIGINT, sig_end);

	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connfd == -1)
	{
		fprintf(stderr, "fail to socket: %s\n", strerror(errno));
		exit(-1);
	}

	memset(&client, 0, sizeof(client));	
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = inet_addr(host);

	ret = connect(connfd, (struct sockaddr *)&client, sizeof(client));
	if(ret == -1)
	{
		fprintf(stderr, "fail to connect: %s\n", strerror(errno));
		exit(-1);
	}

	handle_with_write(connfd);

	exit(0);
}