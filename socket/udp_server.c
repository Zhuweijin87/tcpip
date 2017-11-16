#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <sys/socket.h>
#include <sys/types.h>

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

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    // 注意这里 SOCK_DGRAM
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        fprintf(stderr, "fail to socket:%s", strerror(errno));
        return -1;
    }

    ret = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    if (ret == -1)
    {
        fprintf(stderr, "fail to binf: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}