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

    /// create a sockaddr_in for a given IP address and port
    Address(std::string ip_add, unsigned int short port);

    /// return a sockaddr pointer to the underlying address structure
    sockaddr* operator()() const { return (sockaddr*)&addr; };

    /// generic pointer to the underlying address structure
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

    /// allocate memory
    Data(int num_bytes);

    /// wrap around allocated memory
    Data(const char* data, int len);

    /// create an alias
    /// @note will copy the ownership behaviour of the rhs
    Data& operator=(const Data&);

    /// move and pass on ownership
    Data(Data&&);

    /// release data if owner
    ~Data();

    /// return a pointer to the raw data
    const char* operator()() { return data_ptr; }

    /// generic pointer to the raw data
    template<typename T = const char> T* ptr() { return (T*)data_ptr; }

  private:
    const char* data_ptr = nullptr; ///< raw data
    bool is_owner = false;    ///< true if data internally allocated

    /// free data
    void release();
  };

  //--------------------------------------------------SocketUDP
  /// UDP server/client
  class SocketUDP {
    static constexpr unsigned int short DEFAULT_PORT = 5800;
    static constexpr int MSG_LEN = 1024;

  public:
    /// open a socket
    SocketUDP();

    /// open a socket & bind to an address
    SocketUDP(Address address);

    /// close the socket
    ~SocketUDP();

    /// bind a socket to an address
    void bind(Address address);

    /// wait until data is received
    Data receive(Address& src);

    int send(Data& data, Address& dest);

    int get_file_descriptor() { return fd; }

    Address get_address() { return address; }

  private:
    int fd;
    bool bound = false;
    Address address;
  };
}
