all:server client

server: server.c
	gcc server.c -o server

client: client.c
	gcc client.c -o client

.PHONY: clean s c netstat

clean:
	rm -f server client

s:
	./server

c:
	./client

netstat:
	netstat -a|grep 3000
