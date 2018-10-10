CC=gcc

server: server.o
	$(CC) server.o ./src/utils/utils.c -o server

server.o: 
	$(CC) -c ./src/server_src/server.c -I ./src/utils/

client: client.o
	$(CC) client.o ./src/utils/utils.c -o client

client.o:
	$(CC) -c ./src/client_src/client.c -I ./src/utils/


clean:
	rm -f ./client
	rm -f ./server
	rm -f ./*.o
