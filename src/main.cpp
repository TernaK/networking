#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include "networking.h"

using namespace std;

int main(int argc, char* args[]) {
  networking::Address address("0.0.0.0", 5800);
  networking::SocketUDP socket(address);

  networking::Address src_address;
  networking::Data data = socket.read_data(src_address);
  fprintf(stderr, "from %s:%d\n", src_address.get_ip_address().c_str(), src_address.get_port());
  fprintf(stderr, "message: %s\n", data());
}
