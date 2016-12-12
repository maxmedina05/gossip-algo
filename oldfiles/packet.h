#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>

typedef struct {
  int status;
  char message[100];
  char origin[15];
  char destination[15];
  char filepath[1024];
} Packet;

void printPacket(Packet packet);
Packet buildPacket(std::string message);
char *getMessage(int status);
long toInt(const char* str);

#endif
