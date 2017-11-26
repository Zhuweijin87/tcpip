CC=gcc

EXEC=server client dns arp

all:$(EXEC)

server:server.o
	$(CC) -o $@ server.c

client:client.o
	$(CC) -o $@ client.c 

dns:dns.o
	$(CC) -o $@ dns.c 

arp:arp.o
	$(CC) -o $@ arp.c 

clean:
	rm -f $(EXEC) *.o