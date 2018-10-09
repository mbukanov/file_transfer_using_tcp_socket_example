CC=gcc

server: server.o
	$(CC) server.o -o server

server.o: 
	$(CC) -c ./src/server_src/server.c -I ./src/utils/

client: client.o
	$(CC) client.o -o client

client.o:
	$(CC) -c ./src/client_src/client.c -I ./src/utils/


clean:
	rm -f ./client
	rm -f ./client.o
	rm -f ./server
	rm -f ./server.o
