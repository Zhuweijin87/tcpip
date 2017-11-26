// 多播 客户端

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
    int     sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        fprintf(stderr, "fail to socket\n", strerror(errno));
        return 0;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(8001);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(sockfd, (struct sockadddr *)&local, sizeof(local));
    if(ret)
    {
        fprintf(stderr, "fail to bind: %s\n", strerror(errno));
        return -1;
    }

    int loop = 1;
    ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if(ret < 0)
    {
        fprintf(stderr, "fail to setsockopt: %s\n", strerror(errno));
        return -1;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.88"); // 局部多播地址，路由器不进行转发
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); // 使用默认接口

    // 加入广播组
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if(ret < 0)
    {
        fprintf(stderr, "fail to setsockopt: %s\n", strerror(errno));
        return -1;
    }

    int times = 0;
    socklen_t len = 0;

    char msg[100] = {'\0'};

    for(times=0;;times++)
    {
        len = sizeof(local);
        memset(msg, 0, 100);
        ret = recvfrom(sockfd, msg, 100, 0, (struct sockaddr *)&local, &len);
        if(ret == -1)
        {
            fprintf(stderr, "fail to recvfrom: %s\n", strerror(errno));
            return -1;
        }

        fprintf(stderr, "recv %d message from server: %s\n", times, msg);
        sleep(1);
    }

    // 退出广播组 
    setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sockfd);

    return 0;
}