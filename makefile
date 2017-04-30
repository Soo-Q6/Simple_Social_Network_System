CC=gcc
all:server  client
.PHONY:all

server:server.o ser.o
	$(CC) -o server server.o ser.o
client:client.o cli.o
	$(CC) -o client client.o cli.o
server.o:server.c ser.c ser.h
	$(CC) -c server.c ser.c
client.o:client.c cli.c cli.h
	$(CC) -c client.c cli.c

.PHONY:clean
clean:
	rm *.o server  client	
