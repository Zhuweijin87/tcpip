CC=gcc

EXEC: dns server client

all:$(EXEC)

server:server.o
	$(CC) -o $@ server.c

client:client.o
	$(CC) -o $@ client.c 

dns:dns.o
	$(CC) -o $@ dns.c 

clean:
	rm -f $(EXEC)