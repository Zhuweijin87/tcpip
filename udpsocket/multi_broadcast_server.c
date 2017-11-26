//多播 
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int     sockfd;
    int     n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        fprintf(stderr, "fail to socket\n", strerror(errno));
        return 0;
    }

    struct sockaddr_in  server ;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(8001);

    char msg[] = "this broadcast data";

    while(1)
    {
        n = sendto(sockfd, msg, sizeof(msg), 0, (struct sockaddr*)&server, sizeof(server));
        if(n < 0)
        {
            fprintf(stderr, "fail to sendto: %s\n", strerror(errno)); 
            return -1;
        }
        else
        {
            fprintf(stderr, "send message ing...\n");
        }
        sleep(1);
    }

    return 0;
}
