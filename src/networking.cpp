#include "networking.h"
using namespace networking;

//--------------------------------------------------Address
Address::Address(std::string ip_add, unsigned int short port) {
  addr = sockaddr_in();
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip_add.c_str(), (sockaddr*)&addr.sin_addr);
}

std::string Address::get_ip_address() {
  char* ip_address = inet_ntoa(addr.sin_addr);
  return std::string(ip_address);
}

unsigned short Address::get_port() {
  return htons(addr.sin_port);
}

//--------------------------------------------------Data
Data::Data(int length) {
  data_ptr = new char[length];
  this->length = length;
  is_owner = true;
}

Data::Data(char* data_ptr, int length) {
  this->data_ptr = data_ptr;
  this->length = length;
  is_owner = false;
}

Data::~Data() {
  release();
}

Data& Data::operator=(const Data& data) {
  if(this != &data) {
    release();
    data_ptr = data.data_ptr;
    is_owner = data.is_owner;
  }
  return *this;
}

Data::Data(Data&& data) {
  data_ptr = data.data_ptr;
  is_owner = data.is_owner;
  data.data_ptr = nullptr;
}

void Data::release() {
  if(is_owner && data_ptr) {
    delete data_ptr;
  }
  data_ptr = nullptr;
}

//--------------------------------------------------SocketUDP
SocketUDP::SocketUDP(Address address) : address(address) {
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
    throw std::runtime_error(strerror(errno));

  if(bind(fd, address(), sizeof(address.addr)) == -1)
    throw std::runtime_error(strerror(errno));

  std::cerr << "UDP socket created @ " << address.get_ip_address() << ":" << address.get_port() << std::endl;
}

SocketUDP::~SocketUDP() {
  close(fd);
}

Data SocketUDP::read_data(Address& src) {
  Data data(MSG_LEN);
  socklen_t socklen = sizeof(src.addr);
  data.length = recvfrom(fd, data(), data.length, 0, src(), &socklen);
  return data;
}

int SocketUDP::send_data(Data& data, Address& dest) {
  socklen_t socklen = sizeof(dest.addr);
  int bytes_sent = sendto(fd, data(), data.length, 0, dest(), socklen);
  if(!(bytes_sent == data.length))
    fprintf(stderr, "%d bytes sent of %lu\n", bytes_sent, data.length);
  return bytes_sent;
}
