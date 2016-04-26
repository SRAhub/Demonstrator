#include "demonstrator_bits/network.hpp"

// C++ Standard Library
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <stdexcept>

// Unix library
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
// IWYU pragma: no_include <bits/socket_type.h>

namespace demo {
  Network::Network(
      const unsigned int port) {
    queuedSocketDescriptors_ = ::socket(AF_INET, SOCK_STREAM, 0);

    if (queuedSocketDescriptors_ < 0) {
      throw std::runtime_error("Network: " + static_cast<std::string>(std::strerror(errno)));
    }

    struct ::sockaddr_in socketAddress;
    std::memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(port);
    int status = ::bind(queuedSocketDescriptors_, reinterpret_cast<struct ::sockaddr*>(&socketAddress), sizeof(socketAddress));

    if (status < 0) {
      throw std::runtime_error("Network: " + static_cast<std::string>(std::strerror(errno)));
    }

    ::listen(queuedSocketDescriptors_, 1);
  }

  Network::Network(
      Network&& network) {
    queuedSocketDescriptors_ = network.queuedSocketDescriptors_;
    network.queuedSocketDescriptors_ = -1;
  }

  Network& Network::operator=(
      Network&& network) {
    queuedSocketDescriptors_ = network.queuedSocketDescriptors_;
    network.queuedSocketDescriptors_ = -1;

    return *this;
  }

  Network::~Network() {
    if (queuedSocketDescriptors_ != -1) {
      ::close(queuedSocketDescriptors_);
    }
  }

  void Network::send(
      const std::string& hostname,
      const unsigned int port,
      const std::string& data) {
    int outgoingSocketDescriptor = ::socket(AF_INET, SOCK_STREAM, 0);

    if (outgoingSocketDescriptor < 0) {
      throw std::runtime_error("Network.send: " + static_cast<std::string>(std::strerror(errno)));
    }

    struct ::sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    ::inet_pton(AF_INET, hostname.c_str(), &(serverAddress.sin_addr));
    serverAddress.sin_port = htons(port);
    int status = ::connect(outgoingSocketDescriptor, reinterpret_cast<struct ::sockaddr*>(&serverAddress), sizeof(serverAddress));

    if (status < 0) {
      throw std::runtime_error("Network.send: " + static_cast<std::string>(std::strerror(errno)));
    }

    status = ::write(outgoingSocketDescriptor, data.c_str(), data.size());

    if (status < 0) {
      throw std::runtime_error("Network.send: " + static_cast<std::string>(std::strerror(errno)));
    }
    
    ::close(outgoingSocketDescriptor);
  }

  std::string Network::receive() {
    struct ::sockaddr_in clientAddress;
    std::size_t clientAddressSize = sizeof(clientAddress);
    int incomingSocketDescriptor = ::accept(queuedSocketDescriptors_, reinterpret_cast<struct ::sockaddr*>(&clientAddress), reinterpret_cast<::socklen_t*>(&clientAddressSize));

    if (incomingSocketDescriptor < 0) {
      throw std::runtime_error("Network.receive: " + static_cast<std::string>(std::strerror(errno)));
    }

    std::array<char, 1024> buffer;
    int datasize = ::read(incomingSocketDescriptor, buffer.data(), buffer.size());

    ::close(incomingSocketDescriptor);

    if (datasize < 0) {
      throw std::runtime_error("Network.receive: " + static_cast<std::string>(std::strerror(errno)));
    }

    buffer.at(datasize) = '\0';
    return static_cast<std::string>(buffer.data());
  }
}
