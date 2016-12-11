#include "packet.h"

using namespace std;

void printPacket(Packet packet){
  printf("status: %d\n", packet.status);
  printf("message: %s\n", packet.message);
  printf("origin: %s\n", packet.origin);
  printf("destination: %s\n", packet.destination);
  printf("filepath: %s\n", packet.filepath);
}

Packet buildPacket(string message) {
    int i = 0;
    stringstream ss(message);
    string token;
    Packet packet;
    while(ss >> token) {
      // cout << token << endl;
      switch (i) {
        case 0:
          packet.status = toInt(token.c_str());
          strcpy(packet.message, getMessage(packet.status));
        break;
        case 1:
          strcpy(packet.origin, token.c_str());
        break;
        case 2:
          strcpy(packet.destination, token.c_str());
        break;
        case 3:
          strcpy(packet.filepath, token.c_str());
        break;
      }
      i++;
    }
    return packet;
}

char *getMessage(int status){
  switch (status) {
    case 100:
      return "Got message!";
    case 200:
      return "File was found!";
    case 400:
      return "File was found!";
  }
  return "Wrong status code";
}

long toInt(const char* str)
{
    long mult = 1;
    long re = 0;
    int len = strlen(str);
    int isNeg = 0;
    for(int i = len -1 ; i >= 0 ; i--)
    {
        if(str[i] == '-') {
          isNeg = 1;
          continue;
        }
        re = re + ((int)str[i] -48)*mult;
        mult = mult*10;
    }
    return (isNeg) ? -1*re : re;
}
