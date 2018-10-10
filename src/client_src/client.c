#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "utils.h"

#define DEFAULT_FILENAME "./images/img.tar"

#define DEFAULT_SERVER_ADDRESS "192.168.1.1"
#define DEFAULT_PORT_NUMBER 8888


void print_help() {
	printf("-h : help\n");
	printf("-d : debug\n");
	printf("-f <filename> : filename for upload(default %s)\n", DEFAULT_FILENAME);
	printf("-s <server ip> : set server ip(default %s)\n", DEFAULT_SERVER_ADDRESS);
	printf("-p <server port> : set server port(default %d)\n", DEFAULT_PORT_NUMBER);
}



int main(int argc, char ** argv) {

	int res = 0;
	const int FILENAME_SIZE = 256;
	char filename[FILENAME_SIZE];
	strcpy(filename, DEFAULT_FILENAME);
	char server_ip[32];
	strcpy(server_ip, DEFAULT_SERVER_ADDRESS);
	unsigned short server_port = DEFAULT_PORT_NUMBER;


	while ( (res = getopt(argc,argv,"df:hs:p:")) != -1) {
		switch(res) {
			case 'd':
				setDebug(1);
				break;
			case 'h':
				print_help();
				return 1;
			case 'f':
				memset(filename, 0, FILENAME_SIZE);
				strcpy(filename, optarg);
				break;
			case 's':
				memset(server_ip, 0, 32);
				strcpy(server_ip, optarg);
				break;
			case 'p':
				server_port = atoi(optarg);
				break;
		}
	}
	
	int sockfd = 0;
	struct sockaddr_in server;
	struct stat file_stat;
	int sent_bytes = 0;
	char file_size[256] = "";
	int fd = 0;
	int remain_data;
	int offset = 0;
	memset(&server, 0, sizeof(struct sockaddr_in));

	printer("Server ip = %s\n", server_ip);
	printer("Server port = %d\n", server_port);

	server.sin_family = AF_INET;
	inet_pton(AF_INET, server_ip, &(server.sin_addr));
	server.sin_port = htons(server_port);

	printer("Create socket...\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printer("Could not create socket\n");
		return -1;
	}

	printer("Connection...\n");
	if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
		printer("Connection failure\n");
		close(sockfd);
		return -1;
	}


	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printer("Error opening file %s\n", filename);
		return -1;
	}

	if (fstat(fd, &file_stat) < 0) {
		printer("Error stat file %s\n", filename);
		return -1;
	}
	
	printer("File Size: %d bytes\n", (int)file_stat.st_size);

	sprintf(file_size, "%d", (int)file_stat.st_size);

	int len = send(sockfd, file_size, sizeof(file_size), 0);
	if(len < 0) {
		printer("Failure send len\n");
		return -1;
	}

	printf("Sent %d bytes for the size\n", len);
	remain_data = file_stat.st_size;
	printer("remain_data = %d\n", remain_data);
	while (((sent_bytes = sendfile(sockfd, fd, NULL, 1024)) > 0) && (remain_data > 0)) {
		remain_data -= sent_bytes;
		printf("Server sent %d bytes, left %d bytes\n", sent_bytes, remain_data);
	}

	close(sockfd);

		
	return 0;
}
