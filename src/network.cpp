#include "demonstrator_bits/network.hpp"

// C++ Standard Library
#include <array>
#include <cstring>
#include <cerrno>
#include <stdexcept>

// *Unix
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace demo {
  NetworkAdapter::NetworkAdapter(
      const unsigned int port) {
    incomingSocketDescriptor_ = ::socket(AF_INET, SOCK_STREAM, 0);

    if(incomingSocketDescriptor_ < 0) {
      throw std::runtime_error(std::strerror(errno));
    }

    struct ::sockaddr_in socketAddress;
    std::memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(port);
    int status = ::bind(incomingSocketDescriptor_, reinterpret_cast<struct ::sockaddr*>(&socketAddress), sizeof(socketAddress));

    if(status < 0) {
      throw std::runtime_error(std::strerror(errno));
    }

    // The magic backlog number (511) is based on the default setting in nginx and redis.
    // This may be (silently) reduced to a small number within the underlying operation system.
    ::listen(incomingSocketDescriptor_, 511);
  }

  NetworkAdapter::~NetworkAdapter() {
    if (incomingSocketDescriptor_ != -1) {
      ::close(incomingSocketDescriptor_);
    }
  }

  void NetworkAdapter::send(
      const std::string& hostname,
      const std::string& data) {
    int status = ::write(this->openOutgoingConnectionSocket(), data.c_str(), data.size());

    if(status < 0) {
      throw std::runtime_error(std::strerror(errno));
    }
  }

  void NetworkAdapter::openIncommingConnectionSocket() {
    struct ::sockaddr_in clientAddress;
    std::size_t clientAddressSize = sizeof(clientAddress);
    int incomingSocketDescriptor_ = ::accept(socketDescriptor_, reinterpret_cast<struct ::sockaddr*>(&clientAddress), reinterpret_cast<::socklen_t*>(&clientAddressSize));

    if(incomingSocketDescriptor_ < 0) {
      throw std::runtime_error(std::strerror(errno));
    }
  }

  std::string NetworkAdapter::receive() {
    std::array<char, 1024> buffer;
    int datasize = ::read(incomingSocketDescriptor_, buffer.data(), buffer.size());

    ::close(incomingSocketDescriptor_);

    if(datasize < 0) {
      throw std::runtime_error(std::strerror(errno));
    }

    buffer.at(datasize) = '\0';
    return std::string(buffer.data());
  }

  int NetworkAdapter::establishOutgoingConnectionSocket(
      const std::string& ip,
      const unsigned int port) {
    int socketDescriptor = ::socket(AF_INET, SOCK_STREAM, 0);

    if(socketDescriptor < 0) {
      throw std::runtime_error(std::strerror(errno));
    }

    struct ::sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    ::inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr));
    serverAddress.sin_port = htons(port);
    int status = ::connect(socketDescriptor, reinterpret_cast<struct ::sockaddr*>(&serverAddress), sizeof(serverAddress));

    if(status < 0) {
      throw std::runtime_error(std::strerror(errno));
    }

    return socketDescriptor;
  }

}
