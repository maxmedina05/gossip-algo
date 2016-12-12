#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CONN_LIMIT 3
#define BUFFER_SIZE 1024
#define PORT 3000

void connectionHandler(int socket, char* hostIpAddress);
int isFileHere(char* filepath);

int main(int argc, char *argv[]){

  int socketDesc, newSocket, c;
  struct sockaddr_in server, client;
  char *message;
  char *hostIpAddress = argv[1];
  socketDesc = socket(AF_INET, SOCK_STREAM, 0);
  if(socketDesc == -1) {
    printf("Error: Could not create socket!\n");
    exit(1);
  }
  printf("Socket created!\n");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  if(bind(socketDesc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
    printf("Error: Could not bind socket to port:%d!\n", PORT);
    exit(1);
  }
  printf("Bilding done! %s\n", hostIpAddress);

  if(listen(socketDesc, CONN_LIMIT) < 0) {
    printf("Error: Could not listen on port:%d!\n", PORT);
    exit(1);
  }

  printf("Waiting for incoming connections...\n");
  c = sizeof(struct sockaddr_in);
  while( (newSocket = accept(socketDesc, (struct sockaddr *)&client, (socklen_t*)&c)) )
  {
      printf("Connection accepted\n");
      connectionHandler(newSocket, hostIpAddress);
  }

  if (newSocket < 0)
  {
      printf("Error: Socket wasn't accepted!\n");
      return 1;
  }

  return 0;
}

void connectionHandler(int socket, char* hostIpAddress) {
  int bytesRecived;
  char recvBuffer[BUFFER_SIZE];
  char message[100];
  sprintf(message, "Connection established with %s host.\n", hostIpAddress);
  write(socket, message, strlen(message));

  while((bytesRecived = recv(socket, recvBuffer, BUFFER_SIZE, 0)) > 0) {
    recvBuffer[bytesRecived-2] = '\0';
    if(isFileHere(recvBuffer)) {
      char str[100] = "File was found!";
      write(socket , str , strlen(str));
    } else {
      char str[100] = "File was not found!";
      write(socket , str , strlen(str));
      // Search on another node
    }
  }

  if(bytesRecived == 0) {
    printf("Client disconnected!\n");
    fflush(stdout);
  }
  else if(bytesRecived == -1)
  {
      printf("recv function failed!\n");
  }
}

int isFileHere(char *filepath) {
  return (access(filepath, F_OK) == 0);
}
