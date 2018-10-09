#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdarg.h>
#include "utils.h"

#define DEFAULT_FILENAME "/tmp/img_auto_fw_up"
#define DEFAULT_PORT 8888

void print_help() {
	printf("-h : help\n");
	printf("-d : debug\n");
	printf("-f <filename> : path for image(default %s)\n", DEFAULT_FILENAME);
	printf("-p <port> : set port(default %d)\n", DEFAULT_PORT);
}

int main(int argc, char ** argv) {
	int res = 0;
	const int FILENAME_SIZE = 256;
	char filename[FILENAME_SIZE];
	strcpy(filename, DEFAULT_FILENAME);
	unsigned short server_port = DEFAULT_PORT;
	while ( (res = getopt(argc,argv,"df:hp:")) != -1) { 
		switch(res) { 
			case 'd': 
				DBG = 1;
				break;
			case 'f':
				memset(filename, 0, FILENAME_SIZE);
				strcpy(filename, optarg);
				printer("Set new filename = %s\n", filename);
				break;
			case 'h':
				print_help();
				return 0;
				break;
			case 'p':
				server_port = atoi(optarg);
				break;
		}
	}
	int sockfd = 0;
	int client_sockfd = 0;
	ssize_t len = 0;
	socklen_t socklen = 0;
	const int SIZE = 1024;
	char buffer[SIZE];
	FILE *received_file = NULL;
	struct sockaddr_in server, client;
	memset(&server, 0, sizeof(struct sockaddr_in));
	memset(&client, 0, sizeof(struct sockaddr_in));

	printer("Create socket...\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printer("Could not create socket\n");
		return -1;
	}
	printer("Socket create successfully\n");
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(server_port);

	printer("Binding...\n");
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0 ) {
		printer("Bind soccess\n");
		return -1;
	}

	printer("Listen on port %d ...\n", server_port);
	if(listen(sockfd, 3) == -1) {
		printer("Listen failure\n");
		return -1;
	}

	printer("accept\n");
	client_sockfd = accept(sockfd, (struct sockaddr *)&client, &socklen);
	if(client_sockfd < 0 ) {
		printer("Accept failure\n");
		return -1;
	}

	recv(client_sockfd, buffer, SIZE, 0);
	int file_size = atoi(buffer);
	printer("File size = %d\n", file_size);

	received_file = fopen(filename, "wb");
	if (received_file == NULL) {
		printer("fopen failure\n");
		return -1;
	}

	printer("Receving file...\n");
	int remain_data = file_size;
	while (((len = recv(client_sockfd, buffer, SIZE, 0)) > 0) && (remain_data > 0)) {
		fwrite(buffer, sizeof(char), len, received_file);
		remain_data -= len;
		printer("Receive %d bytes ( left %d bytes ) and write to %s\n", (int)len, remain_data, filename);
	}
	printer("len = %d\n", (int)len);
	fclose(received_file);
	close(client_sockfd);
	close(sockfd);
		


	return 0;
}
