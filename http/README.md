## HTTP 协议 


### Content-Type 
Content-Type 告知客户端返回的数据格式，下面是这些数据类型（也称MIME type)

+ text/plain 
+ text/html
+ text/css 
+ image/jpeg 
+ image/png 
+ image/svg+xml
+ audio/mp4
+ video/mp4 
+ application/javascript 
+ application/pdf 
+ application/zip
+ application/atom+xml

### Content-Length 
返回的数据长度，在1.0版本中不是必须的，因为客户端（浏览器）发现服务器关闭了TCP链接，就表明数据包已经全了。

### Content-Encoding 
数据的编码方式，有下面几种

+ gzip 
+ compress
+ default 

