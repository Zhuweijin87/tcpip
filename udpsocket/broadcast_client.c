// UDP 广播模式客户端 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        fprintf(stderr, "fail to socket: %s\n", strerror(errno));
        return -1;
    }

    int  opt = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if(ret == -1)
    {
        fprintf(stderr, "fail to set sockopt: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in to;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_port = htons(port);
    to.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sockfd, (struct sockaddr*)&to, sizeof(to));
    if (ret == -1)
    {
        fprintf(stderr, "fail to binf: %s\n", strerror(errno));
        return -1;
    }

    socklen_t len = sizeof(to);
    char  msg[100] = {'\0'};

    // 这里会接收到UDP服务端发过来的广播数据 
    while(1)
    {
        ret = recvfrom(sockfd, msg, 100, 0, (struct sockaddr*)&to, &len);
        if(ret < 0)
        {
            fprintf(stderr, "fail to recvfrom: %s\n", strerror(errno));
            return -1;
        }
        else
        {
            fprintf(stderr, "recv: %s\n", msg);
        }
    }
}