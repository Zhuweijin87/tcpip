## UDP 协议 
------------------------- 
使用UDP协议进行信息传输不需要建立连接；  
+ 单播用于两个主机间的端对端通信
+ 广播用于一个主机对整个局域网间通信 

#### UDP广播

广播使用的广播地址为255.255.255.255，与单播地址的区别在于IP不同，本地广播信息不会被路由器转发。  

实例： 服务端：udp_server.c， 客户端： udp_client.c 


#### UDP 多播 

多播也称组播，将统一业务类型的主机进行逻辑上的分组，仅在同一组的主机能接受发送数据。  

多播地址是特定的，D类IP地址多用于多播，即224.0.0.0 - 239.255.255.255之间，主要划分如下：
+ 局部多播地址： 在224.0.0.0 - 224.0.0.255，为路由协议和其他用途保留，路由器并不转发此范围内的IP包 
+ 预留多播地址： 在224.0.1.0 - 238.255.255。255之间，可用于全球范围内网络协议 
+ 管路全线多播地址： 在239.0.0.0 - 239.255.255.255之间，可供组织内部使用，类似私有IP。不能用于Internet，可限制多播范围。 

多播程序实现使用setsockopt()函数和getsockopt()函数 
|选项|含义|
|IP_MULTICAST_TTL| 设置多播数组数据的TTL值|
|IP_ADD_MEMBERSHIP|加入组播组|
|IP_DROP_MEMBERSHIP|退出组播组|
|IP_MULTICAST_IF|获取默认接口或设置接口|
|IP_MULTICAST_LOOP|禁止组播数据回送|  
