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

Data::Data(const char* data_ptr, int length) {
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
  data.is_owner = false;
  data.data_ptr = nullptr;
}

void Data::release() {
  if(is_owner && data_ptr) {
    delete data_ptr;
  }
  data_ptr = nullptr;
}

//--------------------------------------------------SocketUDP
SocketUDP::SocketUDP() {
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
    throw std::runtime_error(strerror(errno));
  std::cerr << "UDP socket created" << std::endl;
}

SocketUDP::SocketUDP(Address address) : SocketUDP() {
  bind(address);
}

SocketUDP::~SocketUDP() {
  close();
}

void SocketUDP::bind(Address address) {
  int value = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
  if(::bind(fd, address(), sizeof(address.addr)) == -1)
    throw std::runtime_error(strerror(errno));
  this->address = address;
  bound = true;
  fprintf(stderr, "socket bound to %s:%d\n", address.get_ip_address().c_str(), address.get_port());
}

Data SocketUDP::receive(Address& src) {
  if(!bound)
    std::cerr << "socket not bound" << std::endl;
  Data data(MSG_LEN);
  socklen_t socklen = sizeof(src.addr);
  data.length = recvfrom(fd, data.ptr<void>(), data.length, 0, src(), &socklen);
  return data;
}

int SocketUDP::send(Data& data, Address&& dest) {
  socklen_t socklen = sizeof(dest.addr);
  int bytes_sent = sendto(fd, data.ptr<void>(), data.length, 0, dest.ptr<sockaddr>(), socklen);
  if(!(bytes_sent == data.length))
    fprintf(stderr, "%d bytes sent of %lu\n", bytes_sent, data.length);
  return bytes_sent;
}

//--------------------------------------------------ServerUDP
ServerUDP::ServerUDP(Address address, CallbackFunc& callback) {
  socket = std::unique_ptr<SocketUDP>(new SocketUDP(address));
  data_callback = callback;
  server_thread = std::thread(&ServerUDP::server_routine, this);
}

ServerUDP::~ServerUDP() {
  stop();
}

void ServerUDP::stop() {
  run = false;
  if(server_thread.joinable())
    server_thread.join();
}

void ServerUDP::server_routine() {
  while(run) {
    Address src_address;
    Data data = socket->receive(src_address);
    data_callback(data,src_address);
  }
}
