#include "networking.h"

using namespace std;

int main(int argc, char* args[]) {

  networking::ServerUDP::CallbackFunc
  server_callback = [](networking::Data& data, networking::Address& address) -> void {
    fprintf(stderr, "from %s:%d\n", address.get_ip_address().c_str(), address.get_port());
    std::string message(data(), data.length);
    fprintf(stderr, "message: %s\n", message.c_str());
  };

  networking::ServerUDP server({"0.0.0.0", 5800}, server_callback);

  networking::SocketUDP socket;
  for(size_t loop = 0; loop < 5; loop++) {
    this_thread::sleep_for(chrono::milliseconds(200));
    std::string message = "hello server " + to_string(loop);
    networking::Data data(message.c_str(), message.length());
    socket.send(data, {"127.0.0.1", 5800});
  }
  socket.close();

  while(server.is_running())
    this_thread::sleep_for(chrono::milliseconds(2000));
}
