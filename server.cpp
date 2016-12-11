/*
  author: Max Medina
  create date: 2016-12-10
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "myconfig.h"

int callback(int arg, int arg2){
  printf("I finished! %d\n", arg);
  printf("I finished! %d\n", arg2);
  return 0;
}

void func(int(*success)(int, int)){
  success(5, 10);
}

void sendMessage(int j, int i, int sockfd, int bytesRecived, char* buffer,
   fd_set *master) {
  if(FD_ISSET(j, master)) {
    if(j != sockfd && j != i) {
      if(send(j, buffer, bytesRecived, 0) == -1){
        perror("Cannot broadcast message!");
      }
    }
  }
}

void connectionHandler(int i, fd_set *master, int sockfd, int fdmax){
  int bytesRecived, j;
  char buffer[BUFFER_SIZE];

  if((bytesRecived = recv(i, buffer, BUFFER_SIZE, 0)) <= 0){
    if(bytesRecived == 0){
      printf("socket %d hung up\n", i);
    } else {
      perror("Something happended while waiting for data!");
    }
    close(i);
    FD_CLR(i, master);
  } else {
    for (j = 0; j <= fdmax; j++) {
      sendMessage(j, i, sockfd, bytesRecived, buffer, master);
    }
  }
}

void acceptConnection(fd_set* master, int *fdmax, int sockfd, struct sockaddr_in *client_addr) {
  socklen_t addrlen;
  int newsockfd;

  addrlen = sizeof(struct sockaddr_in);
  if((newsockfd = accept(sockfd, (struct sockaddr*)client_addr, &addrlen)) == -1){
    perror("Cannot establish connection!");
    exit(1);
  } else {
    FD_SET(newsockfd, master);
    if(newsockfd > *fdmax){
      *fdmax = newsockfd;
    }
    printf("New connection from %s on port %d.\n",
    inet_ntoa(client_addr->sin_addr),
    ntohs(client_addr->sin_port));
  }
}

void serverConfigInit(int *sockfd, struct sockaddr_in *server_addr) {
  int yes = 1;
  if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Error while configurating the server.\n");
    exit(1);
  }

  server_addr->sin_family = AF_INET;
  server_addr->sin_port = htons(PORT);
  server_addr->sin_addr.s_addr = INADDR_ANY;
  memset(server_addr->sin_zero, '\0', sizeof(server_addr->sin_zero));

  if(setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("Error while setting the socket options\n");
    exit(1);
  }

  if(bind(*sockfd, (struct sockaddr*)server_addr, sizeof(struct sockaddr)) == -1) {
    perror("Error while binding\n");
    exit(1);
  }

  if(listen(*sockfd, 10) == -1) {
    perror("cannot listen on that socket\n");
    exit(1);
  }

  printf("\nTCPServer Waiting for clients on port %d\n", PORT);
  fflush(stdout);
}

int main(int argc, char* args[]){

  fd_set master;
  fd_set read_fds;
  int fdmax, i;
  int sockfd = 0;
  struct sockaddr_in server_addr, client_addr;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  serverConfigInit(&sockfd, &server_addr);
  FD_SET(sockfd, &master);

  fdmax = sockfd;
  while(FOREVER){
    read_fds = master;
    if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
      perror("Error on select");
      exit(4);
    }
    for(i = 0; i <= fdmax; i++){
      if(FD_ISSET(i, &read_fds)) {
        if(i == sockfd){
          acceptConnection(&master, &fdmax, sockfd, &client_addr);
        } else {
          connectionHandler(i, &master, sockfd, fdmax);
        }
      }
    }
  }

  return 0;
}
