#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include "networking.h"

struct Server {
  int fd;
  std::atomic_bool stop { true };
  Server(std::string ip_addr="0.0.0.0", unsigned short port=5800) {
    int FAMILY = AF_INET;
    int SOCK_TYPE = SOCK_DGRAM;
    int MAX_LEN = 1024;
    sockaddr_in addr = sockaddr_in();
    addr.sin_family = FAMILY;
    addr.sin_port = htons(port);
    if(inet_pton(FAMILY, ip_addr.c_str(), &addr.sin_addr) == 0)
      throw std::runtime_error(strerror(errno));

    fd = socket(FAMILY, SOCK_TYPE, 0);
    if(fd == -1)
      throw std::runtime_error(strerror(errno));
    fprintf(stderr, "socket created: %d\n", fd);
    printf("bound to %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
      throw std::runtime_error(strerror(errno));

    stop = false;
    while(!stop) {
      char buffer[MAX_LEN];
      sockaddr_in client_addr = sockaddr_in();
      socklen_t socklen = sizeof(client_addr);
      int bytes_received = recvfrom(fd, buffer, MAX_LEN, 0, (sockaddr*)&client_addr, &socklen);
      if(bytes_received) {
        std::string data(buffer, bytes_received);

        printf("from to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        fprintf(stderr, "%d bytes received\n", bytes_received);
        std::cerr << data << std::endl;
      }
    }
  }
};

using namespace std;

int main(int argc, char* args[]) {
  networking::Address address("0.0.0.0", 5800);
  networking::SocketUDP socket(address);

  networking::Address src_address;
  networking::Data data = socket.read_data(src_address);
  fprintf(stderr, "from %s:%d\n", src_address.get_ip_address().c_str(), src_address.get_port());
  fprintf(stderr, "message: %s\n", data());
}
