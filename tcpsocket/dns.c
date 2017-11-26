// 域名解析 
// DNS : 域名解析系统 

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int args, char *argv[])
{
    char *host = NULL;
    char ch = '\0';
    struct hostent *ht = NULL;

    while((ch=getopt(args, argv, ":s:p")) != -1)
    {
        switch(ch)
		{
		case 's':
			host = optarg;
			break;
		default:
			break;
		}
    }

    ht = gethostbyname(host);
    if(ht)
    {
        int  i = 0;
        printf("origin host address:%s\n", host);
        printf("name:%s\n", ht->h_name);
        printf("type:%s\n", ht->h_addrtype==AF_INET?"AF_INET":"AF_INET6");

        printf("length:%d\n", ht->h_length);

        // IP地址
        for(i=0; ; i++)
        {
            if(ht->h_addr_list[i] != NULL)
            {
                printf("IP:%s\n", inet_ntop(ht->h_addrtype, ht->h_addr_list[i], str, 30));
            } 
            else
            {
                break;
            }
        }

        // 域名地址打印 
        for(i=0; ;i++)
        {
            if(ht->h_aliases[i] != NULL)
            {
                printf("alias %d:%s\n", i, ht->h_aliases[i]);
            }
            else
            {
                break;
            }
        }
        return 0;
    }
}

// 注意：gethostbyname函数不可重入
