// 获取电脑MAC地址信息

#include <stdio.h>        
#include <sys/types.h>  
#include <ifaddrs.h>  
#include <netinet/in.h>   
#include <string.h>   
#include <arpa/inet.h>  
  
int main (int argc, const char *argv[]) 
{  
    struct ifaddrs * ifAddrStruct=NULL;  
    void* tmpAddrPtr=NULL;  
  
    getifaddrs(&ifAddrStruct);  
  
    while (ifAddrStruct!=NULL) {  
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET) 
        { 
            // check it is IP4  
            // is a valid IP4 Address  
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;  
            char addressBuffer[INET_ADDRSTRLEN];  
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);  
            printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer);   
        } 
        else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) 
        { 
            // check it is IP6  
            // is a valid IP6 Address  
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;  
            char addressBuffer[INET6_ADDRSTRLEN];  
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);  
            printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer);   
        }   
        ifAddrStruct=ifAddrStruct->ifa_next;  
    }  
    return 0;  
}  