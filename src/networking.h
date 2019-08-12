#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

namespace networking {
  //--------------------------------------------------Address
  /// Hold an IP4 address
  struct Address {
    sockaddr_in addr;

    Address() = default;
    Address(std::string ip_add, unsigned int short port);
    sockaddr* operator()() const { return (sockaddr*)&addr; };
    template<typename T = sockaddr> T* ptr() { return (T*)&addr; }
    std::string get_ip_address();
    unsigned short get_port();
  };

  //--------------------------------------------------Data
  /// Wrap raw data
  /// @note will free data only if internally allocated
  class Data {
  public:
    size_t length = 0;        ///< length of data

    Data() = default;
    Data(int len);
    Data(const char* data, int len);
    Data& operator=(const Data&);
    Data(Data&&);
    ~Data();
    const char* operator()() { return data_ptr; }
    template<typename T = const char> T* ptr() { return (T*)data_ptr; }

  private:
    const char* data_ptr = nullptr; ///< raw data
    bool is_owner = false;    ///< true if data internally allocated

    /// free data
    void release();
  };

  //--------------------------------------------------SocketUDP
  ///
  class SocketUDP {
    static constexpr unsigned int short DEFAULT_PORT = 5800;
    static constexpr int MSG_LEN = 1024;

  public:
    SocketUDP();

    SocketUDP(Address address);

    ~SocketUDP();

    void bind(Address address);

    Data receive(Address& src);

    int send(Data& data, Address& dest);

    int get_file_descriptor() { return fd; }

  private:
    int fd;
    bool bound = false;
    Address address;
  };
}
