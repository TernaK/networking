#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include "networking.h"

using namespace std;

int main(int argc, char* args[]) {
  std::thread server_thread([]() {
    networking::Address address("0.0.0.0", 5800);
    networking::SocketUDP socket(address);

    while(true) {
      networking::Address src_address;
      networking::Data data = socket.receive(src_address);
      fprintf(stderr, "from %s:%d\n", src_address.get_ip_address().c_str(), src_address.get_port());
      std::string message(data(), data.length);
      fprintf(stderr, "message: %s\n", message.c_str());
    }
  });

  this_thread::sleep_for(chrono::seconds(2));
  networking::SocketUDP socket;
  networking::Address dst_address("127.0.0.1", 5800);

  for(size_t loop = 0; loop < 5; loop++) {
    std::string message = "hello world: " + to_string(loop);
    networking::Data data(message.c_str(), message.length());
    socket.send(data, dst_address);
  }
  close(socket.get_file_descriptor());

  server_thread.join();
}
