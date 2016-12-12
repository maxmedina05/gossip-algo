/*
  author: Max Medina
  create date: 2016-12-10
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "myconfig.h"
#include "packet.h"

void connectionHandler(int i, int sockfd) {
  char sendBuffer[BUFFER_SIZE];
  char recvBuffer[BUFFER_SIZE];
  int bytesRecived;
  Packet packet;

  if(i == 0){
    fgets(sendBuffer, BUFFER_SIZE, stdin);
    if(strcmp(sendBuffer, "quit()\n") == 0) {
      exit(0);
    } else {
      send(sockfd, sendBuffer, strlen(sendBuffer), 0);
    }
  } else {
    bytesRecived = recv(sockfd, recvBuffer, BUFFER_SIZE, 0);
    recvBuffer[bytesRecived] = '\0';
    // printf("%s", recvBuffer);
    packet = buildPacket(recvBuffer);
    
    printPacket(packet);

    fflush(stdout);
  }
}

void clientConfigInit(int *sockfd, struct sockaddr_in *server_addr, char* serverIpAddress) {
  if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Error while configurating the client.\n");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = inet_addr(serverIpAddress);
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);

	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("Error while connecting to the server");
		exit(1);
	}
}

int main(int argc, char *argv[]) {

  if(argc < 2){
    printf("No argument pass\n");
    exit(1);
  }

  int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;

	clientConfigInit(&sockfd, &server_addr, argv[1]);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;

	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}

		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				connectionHandler(i, sockfd);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
