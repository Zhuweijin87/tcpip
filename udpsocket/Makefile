CC=gcc
CFLAG=

target=udp_server udp_client multibroad_server multibroad_client 

all:$(target)

udp_server:broadcast_server.o
	$(CC) -o $@ broadcast_server.o

udp_client:broadcast_client.o 
	$(CC) -o $@ broadcast_client.o 

multibroad_server:multi_broadcast_server.o
	$(CC) -o $@ multi_broadcast_server.o 

multibroad_client:multi_broadcast_client.o
	$(CC) -o $@ multi_broadcast_client.o
	
clean:
	rm -f $(target) *.o