#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CONN_LIMIT 3
#define BUFFER_SIZE 1024
#define PORT 3000

void connectionHandler(int socket, char* hostIpAddress, char* friends[], int cf);
int isFileHere(char* filepath);
int askFriend(char* friendAddr, char* filepath);

int main(int argc, char *argv[]){
  int friendsCount = 2;
  char *friends[] = {
    "10.0.0.19",
    "10.0.0.34"
  };

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
      connectionHandler(newSocket, hostIpAddress, friends, friendsCount);
  }

  if (newSocket < 0)
  {
      printf("Error: Socket wasn't accepted!\n");
      return 1;
  }

  close(socketDesc);
  return 0;
}

void connectionHandler(int socket, char* hostIpAddress, char* friends[], int cf) {
  int bytesRecived;
  char recvBuffer[BUFFER_SIZE];
  char message[100];
  char lastMessage[100];
  // sprintf(message, "Connection established with %s host.\n", hostIpAddress);
  // write(socket, message, strlen(message));

  while((bytesRecived = recv(socket, recvBuffer, BUFFER_SIZE, 0)) > 0) {
    // printf("buffersize: %d\n", bytesRecived);

    recvBuffer[bytesRecived] = '\0';
    if(strcmp(lastMessage, recvBuffer) != 0) {
      strcpy(lastMessage, recvBuffer);
      // printf("recvBuffer: %s\n", recvBuffer);
      char str[100];
      if(isFileHere(recvBuffer)) {
         sprintf(str, "1 - File was found on %s!\n", hostIpAddress);
        write(socket , str , strlen(str));
      } else {
        sprintf(str, "1 - File was not found on %s!\n", hostIpAddress);
        write(socket , str , strlen(str));

        // Search on another node
        for(int i = 0; i < cf; i++){
          if(strcmp(hostIpAddress, friends[i]) == 0) continue;
          askFriend(friends[i], recvBuffer);
        }
      }
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

int askFriend(char* friendAddr, char* filepath){
  // if(strcmp(sendBuffer, "quit()\n")
  int sock;
  struct sockaddr_in friendServer;
  char* message, friendReply[BUFFER_SIZE];
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1) {
    printf("Error: Could not create socket!\n");
    exit(1);
  }

  friendServer.sin_addr.s_addr = inet_addr(friendAddr);
  friendServer.sin_family = AF_INET;
  friendServer.sin_port = htons(PORT);

  if(connect(sock, (struct sockaddr *)&friendServer, sizeof(friendServer)) < 0){
    printf("Error: Could not connect to %s remote server!\n", friendAddr);
    exit(1);
  }
  printf("Connected with %s\n", friendAddr);

  if(send(sock, filepath, strlen(filepath), 0) < 0) {
    printf("Error: Could not send data to remote server!\n");
    exit(1);
  }

  if(recv(sock, friendReply, BUFFER_SIZE, 0) < 0) {
    printf("Error: Something happened trying to received reponse\n");
    exit(1);
  }

  printf("friend %s reply: %s\n",friendAddr, friendReply);
  fflush(stdout);
  close(sock);
  int res = friendReply[0] - '0';
  return res;
}
