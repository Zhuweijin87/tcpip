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
#include <fcntl.h>
#include <netinet/tcp.h> /* TCP_KEEPALIVE 等宏的定义 */

int status = 1;

void sig_pipe(int signo)
{
	printf("Catch SIG_PIPE signal\n");
	status = 0;
}

void sig_term(int signo)
{
	printf("Catch SIG_INT signal\n");
	status = 0;
	exit(1);
}

/* 设置服务端连接属性 */
int set_socket_opt(int sockfd)
{
	int             ret;

	/* SO_EXCLUSIVEADDRUSE: 独占端口
 	 * SO_REUSEADDR: 复用端口*/
	/*reuse socket fd when closing the sock_fd in establish status*/
	int opt = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if(ret < 0){
		fprintf(stderr, "fail to set option to SO_REUSEADDR.(%d:%s)\n", errno, strerror(errno));
		return ret;
	}

	/*set send timeout when network exceptions occur*/
	int sndTimeout = 1000;  /* milseconds */
	ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &sndTimeout, sizeof(sndTimeout));
	if(ret < 0){
		fprintf(stderr, "fail to set option to SO_SNDTIMEO.(%d:%s)\n", errno, strerror(errno));
		return ret;
	}

	/*set recv timeout where network exceptions occur*/
	int rcvTimeout = 1000;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &rcvTimeout, sizeof(rcvTimeout));
	if(ret < 0){
		fprintf(stderr, "fail to set option to SO_RCVTIMEO.(%d:%s)\n", errno, strerror(errno));
		return ret;
	}

	/*set send buffer size, system default about 8688 bytes, if buffer size is set to 0,
     buffer will not copy to system cache, it should be set before listen() or connect()*/
	int sndBufSize = 10 * 1024; /* 10KB */
	ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndBufSize, sizeof(sndBufSize));
	if(ret < -1){
		fprintf(stderr, "fail to set option to SO_SNDBUF.\n");
		return ret;
	}

	/*set recv buffer size, as like above*/
	int rcvBufSize = 10 * 1024;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvBufSize, sizeof(rcvBufSize));
	if(ret < 0){
		fprintf(stderr, "fail to set option to SO_RCVBUF.\n");
		return ret;
	}

	return 0;
}

/* 设置非阻塞 */
int set_socket_noblock(int sockfd)
{
	int		flag, ret;
	/* 获取套接字属性 */
	flag = fcntl(sockfd, F_GETFL, 0);	
	if(flag < 0)
	{
		fprintf(stderr, "fail to get F_GETFL:%s\n", strerror(errno));
		return -1;
	}

	flag |= O_NONBLOCK;
	ret = fcntl(sockfd, F_SETFL, flag);
	if(ret < 0)
	{
		fprintf(stderr, "fail to set F_SETFL: %s\n", strerror(errno));
	}
	return 0;
}

/* 设置心跳包：TCP/IP可以通过内置的参数来设置心跳包，即SO_KEEPALIVE
 * 默认情况下不会开启，开启后可以在规定的时间内向对方发送心跳包，
 * 时间一般默认为 2 小时，探测次数为5次，此参数可以配置 */
int tcp_set_keep_alive(int sockfd, int idleVal, int interVal, int count)
{
	int		ret;

	/* 开启KEEPALIVE功能 */
	int     tag = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &tag, sizeof(tag));
	if(ret == -1)
	{
		fprintf(stderr, "fail to set SO_KEEPALIVE: %s\n", strerror(errno));
		return -1;
	}
	
	/* 配置探测时间 */
#if 0
	/* 使用 IPPROTO_TCP */
	int keepIdle = idleVal;
	ret = setsocketopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE, &keepIdle, sizeof(keepIdle));
	if(ret == -1)
	{
		fprintf(stderr, "fail to set TCP_KEEPALIVE: %s\n", strerror(errno));
		return -1;
	}
#endif

#if 1
	/* 首次探测的时间间隔 */
	int keepIdle = idleVal;
	ret = setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle));
	if(ret == -1)
	{
		fprintf(stderr, "fail to set TCP_KEEPIDLE: %s\n", strerror(errno));
		return -1;
	}

	/* 探测间隔时间 */
	int keepInterVal = interVal;
	ret = setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, &keepInterVal, sizeof(keepInterVal));
	if(ret == -1)
	{
		fprintf(stderr, "fail to set TCP_KEEPINTVL: %s\n", strerror(errno));
		return -1;
	}

	/* 探测次数 */
	int keepCount = count;
	ret = setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepIdle));
	if(ret == -1)
	{
		fprintf(stderr, "fail to set TCP_KEEPCNT: %s\n", strerror(errno));
		return -1;
	}
#endif
	return 0;
}

int handle_client_with_recv(int clifd)
{
	int 	n;
	char	buffer[512] = {'\0'};

	while(status)
	{
		memset(buffer, 0, sizeof(buffer));
		/* 	最后一个参数:	MSG_DONTWAIT: 非阻塞
  							MSG_PEEK: 查看数据, 数据不会丢弃(一般情况下不建议使用)
							MSG_WAITALL: 告诉内核在没有读到请求的字节数之前不使读操作返回
							MSG_TRUNC: 复制缓存区大小的数据，其他数据会被丢弃
			默认： 0 */
		n = recv(clifd, buffer, 30, 0);
		if(n == -1)
		{
			if(errno == EAGAIN)
			{
				continue;
			}

			if(errno == ETIMEDOUT)
			{
				fprintf(stderr, "客户端断开");
			}

			fprintf(stderr, "recv fail: %s\n", strerror(errno));
			close(clifd);
			return -1;
		}
		else if(n == 0)
		{
			fprintf(stderr, "client close\n");
			close(clifd);
			return 0;
		}
		else
		{
			fprintf(stderr, "recv: %s\n", buffer);
		}
	}
}

/* 客户端读写 */
int handle_client_with_read(int clifd)
{
	int		n;
	char	buffer[512] = {'\0'};
	
	while(1)
	{
		n = read(clifd, buffer, 512); /* 自带阻塞 */
		if(n == -1)
		{
			fprintf(stderr, "fail to read: %s\n", strerror(errno));
			close(clifd);
			return 0;
		}
		else if(n == 0)
		{
			fprintf(stderr, "client disconnect\n");
			close(clifd);
			return 0;
		}
		else
		{
			fprintf(stderr, "recv: %s\n", buffer);
		}
	}

	return 0;
}

/* 处理链接请求 */
int handle_connection(int sockfd)
{
	int		clifd;
	struct	sockaddr_in 	client;
	socklen_t	clitlen;

	for( ; ; )
	{
		memset(&client, 0, sizeof(client));
		clitlen = sizeof(client);
		clifd = accept(sockfd, (struct sockaddr *)&client, &clitlen);
		if(clifd == -1)
		{
			if(errno == EAGAIN)
			{
				continue;
			}
			fprintf(stderr, "accept fail: %s\n", strerror(errno));
			return -1;
		}
		
		handle_client_with_recv(clifd);
	}
}

int main(int args, char *argv[])
{
	int		ret;
	int 	sockfd;
	char	ch;
	char	*host = "192.168.1.46";
	int		port = 8010;
	struct sockaddr_in	server;

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

	/* 信号处理 */
	signal(SIGINT, sig_term);
	signal(SIGPIPE, sig_pipe);

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

	tcp_set_keep_alive(sockfd, 10, 3, 3);

	ret = listen(sockfd, 20);
	if(ret == -1)
	{
		fprintf(stderr, "fail to listen: %s\n", strerror(errno));
		exit(1);
	}

	handle_connection(sockfd);

	exit(0);
}
