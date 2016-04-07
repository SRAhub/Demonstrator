#include <iostream>

#include "demonstrator_bits/network.hpp"

#define SERVER_PORT	8001
#define CLIENT_PORT	9001


int main (int argc, char **argv) {
  if (argc < 2 || (argc >= 2 && std::string(argv[1]) != "client" && std::string(argv[1]) != "server")) {
    std::cout << "Use  " << argv[0] << " (client|server)  to specify the mode." << std::endl;
    exit (1);
  }

  if (std::string(argv[1]) == "server") {
    demo::NetworkAdapter server(std::string("soerwer"), SERVER_PORT);
    int fd = -1;
    std::string msg = "";

    do {
      fd = server.openIncommingConnectionSocket();
      msg = server.receive(fd);
      std::cout << msg << std::endl;
    } while (msg != "exit");
  }



  if (std::string(argv[1]) == "client") {
    demo::NetworkAdapter client(std::string("klaient"), CLIENT_PORT);
    int fd = -1;
    std::string msg = "";
    do {
      std::getline(std::cin, msg);
      fd = client.establishOutgoingConnectionSocket("127.0.0.1", SERVER_PORT);
      client.send(fd, msg);
    } while (msg != "exit");
  }

  return EXIT_SUCCESS;
}
