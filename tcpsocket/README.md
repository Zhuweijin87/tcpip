## Linux Socket 编程 
----------

### listen()
由TCP服务端调用 
```
int listen(int sockfd, int backlog)
``` 
调用该函数套接字由CLOSED状态转LISTEN状态。  
这里主要讲下backlog参数的意义：linux内核为任意一个给定的套接字维护两个队列：已完成链接的队列和未完成链接的队列。
+ 已完成链接队列：完成TCP三次握手的每一个过程，该套接字处于ESTABLISHED状态。
+ 未完成链接队列：每个这样的SYN分节对应一项；已由某个客户发出并到达服务器，而服务器正在等待完成相应的TCP三次握手，此套接口处于SYN_RCVD状态。  
图示： 
![Listne状态图]https://github.com/Zhuweijin87/tcpip/png/listen_mode.png

参考链接： https://www.cnblogs.com/lengender-12/archive/2017/05/05/6813057.html  


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


### dns 域名解析 
域名解析所用到的函数：gethostbyname()
