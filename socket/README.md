## Linux Socket 编程 
----------

### 信号处理
如果正在写入套接字的时候，当读取段已经关闭时，可以得到SIGPIPE信号。

### block 与 non-block 
服务端常常把sockfd fd设置为nonblock，是因为采用block IO的模式服务端为每一个client connect创建一个线程模式，这样开销巨大，且扩展性不好，一般通用的做法是：线程池 + nonblock IO + 多路复用(select/poll, epoll(Linux下独有)， kqueue(Free BSD))。

```C
// 常用设置模式
int set_nonblocking(int sockfd)
{
    int  flags;
    if ((flags == fcntl(sockfd, F_GETFL, 0)) == -1)
        return 0;
    return fcntl(sokcfd, F_SETFL, flags | O_NONBLOCK);
}
``` 
1. read总是在接收到缓存区有数据时立即返回，而不是等到给定的read buffer填满时返回。当接受返回区为空时，blocking模式才会等待，而nonblocking模式下回立即返回-1（errno=EAGAIN或EWOULDBLOCK）

2. blocking的write只有在缓冲区足以放下整个buffer时才返回（与blocking read并不相同），nonblocking write则是返回能够放下字节数，之后调用则会返回-1（errno=EAGAIN或EWOULDBLOCK）

