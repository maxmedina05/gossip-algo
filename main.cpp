#include <iostream>
#include <string>

#include "packet.h"

using namespace std;

int main(int argc, char *argv[]){
  string message = "100 10.0.0.34 127.0.0.1 archivo.txt";

  Packet packet = buildPacket(message);

  printPacket(packet);
  return 0;
}
