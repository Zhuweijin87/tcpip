// UDP 广播模式 : 接受局域网内所有人消息
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int args, char *argv[])
{
    char ch = '\0';
    char *host = "127.0.0.1";
    int  port = 8001;
    int  sockfd = 0;
    int  ret;

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

    struct sockaddr_in  server;

    // 注意这里 SOCK_DGRAM
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        fprintf(stderr, "fail to socket:%s", strerror(errno));
        return -1;
    }

    // 设置广播模式 
    int opt = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if(ret == -1)
    {
        fprintf(stderr, "fail to set sockopt: %s\n", strerror(errno));
        return -1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_BROADCAST); // 套接字地址为广播模式

/*
    ret = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    if (ret == -1)
    {
        fprintf(stderr, "fail to binf: %s\n", strerror(errno));
        return -1;
    }
*/

    char  msg[100] = "hello, this is message from server";
    while(1)
    {
        ret = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, sizeof(server));
        if(ret < 0)
        {
            fprintf(stderr, "fail to sendto: %s\n", strerror(errno));
            return -1;
        }
        else
        {
            fprintf(stderr, "ok\n");
        }
        sleep(1);
    }
    return 0;
}