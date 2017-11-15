#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_EVENTS		1024
void add_events(int epollfd, int fd, int state)
{
	struct epoll_event  ev;
	
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void del_events(int epollfd, int fd, int state)
{
	struct	epoll_event	ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void mod_events(int epollfd, int fd, int state)
{
	struct	epoll_event	ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}


int socket_listen(int port, char *host)
{
	int			ret;
	int			listenfd;
	struct sockaddr_in server;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1)
	{
		fprintf(stderr, "fail to socket: %s\n", strerror(errno));
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(host);

	ret = bind(listenfd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1)
	{
		fprintf(stderr, "fail to bind: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	ret = listen(listenfd, 20);
	if(ret == -1)
	{
		fprintf(stderr, "fail to listen: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	return listenfd;
}

int epoll_handle(int listenfd)
{
	int		i, fd;
	int		epollfd, nfds;
	struct epoll_event  events[MAX_EVENTS];
	
	epollfd = epoll_create(MAX_EVENTS);
	if(epollfd == -1)
	{
		fprintf(stderr, "fail to epoll_create: %s\n", strerror(errno));
		return -1;
	}

	add_events(epollfd, listenfd, EPOLLIN);

	while(1)
	{
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, 0);
		if(nfds == -1)
		{
			fprintf(stderr, "fail to epoll_wait: %s\n", strerror(errno));
			return -1;
		}

		if(nfds == 0)
		{
			continue;
		}

		for(i = 0; i < nfds; i++)
		{
			fd = events[i].data.fd;
			if((fd == listenfd) && (events[i].events & EPOLLIN))
			{
				/* handle accept */
				handle_accept(epollfd, listenfd);
			}
			else if(events[i].events & EPOLLIN)
			{
				/* handle read */
				handle_read(epollfd, fd);
			}
			else if(events[i].events & EPOLLOUT)
			{
				/* handle recv */
				handle_write(epollfd, fd);
			}
			else
			{
				/* handle error */
				;
			}
		}	
		
	}
	return 0;		
}

/* 处理客户端连接 */
int handle_accept(int epollfd, int listenfd)
{
	int		ret, connfd;
	struct	sockaddr_in	client;
	socklen_t	client_len;

	client_len = sizeof(struct sockaddr);
	
	connfd = accept(listenfd, (struct sockaddr *)&client, &client_len);
	if(connfd == -1)
	{
		fprintf(stderr, "fail to accept: %s\n", strerror(errno));
		return -1;
	}
	fprintf(stderr, "accept new client: [%s][%d]\n", inet_ntoa(client.sin_addr), client.sin_port);
	//add_events(epollfd, connfd, EPOLLIN);
	add_events(epollfd, connfd, EPOLLIN | EPOLLET);
	return 0;
}

/* 处理读写 */
int handle_read(int epollfd, int readfd)
{
	int		len;
	char	buffer[1025];

	len = read(readfd, buffer, 1024);
	if(len == -1)
	{
		fprintf(stderr, "fail to read: %s\n", strerror(errno));
		del_events(epollfd, readfd, EPOLLIN);
		close(readfd);
		return -1;
	}
	else if(len == 0)
	{
		fprintf(stderr, "close client [%d]\n", readfd);
		del_events(epollfd, readfd, EPOLLIN);
		close(readfd);
	}
	else
	{
		fprintf(stderr, "recv: %s\n", buffer);
		//mod_events(epollfd, readfd, EPOLLOUT);
	}

	return 0;
}

int handle_write(int epollfd, int writefd)
{
	int	len = 0;
	len = write(writefd, "callback", 9);	
	if(len < 0)
	{
		fprintf(stderr, "fail to write: %s\n", strerror(errno));
		del_events(epollfd, writefd, EPOLLOUT);
		return -1;
	}
	else
	{
		mod_events(epollfd, writefd, EPOLLIN);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int		listenfd;

    char	*host = "127.0.0.1";
	int		 port = 8001;
	char	 ch;

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
    
	listenfd = socket_listen(port, host);

	epoll_handle(listenfd);

	return 0;
}
