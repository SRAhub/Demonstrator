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

NetworkAdapter::NetworkAdapter(
    const unsigned int port) {
  socketDescriptor_ = ::socket(AF_INET, SOCK_STREAM, 0);

  if(socketDescriptor_ < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  
  struct ::sockaddr_in socketAddress;
  std::memset(&socketAddress, 0, sizeof(socketAddress));
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_addr.s_addr = INADDR_ANY;
  socketAddress.sin_port = ::htons(port);
  int status = ::bind(socketDescriptor_, reinterpret_cast<struct ::sockaddr*>(&socketAddress), sizeof(socketAddress));

  if(status < 0) {
    throw std::runtime_error(std::strerror(errno));
  }

  // The magic backlog number (511) is based on the default setting in nginx and redis.
  // This may be (silently) reduced to a small number within the underlying operation system.
  ::listen(socketDescriptor_, 511);
}

NetworkAdapter::~NetworkAdapter() {
  if (socketDescriptor_ != -1) {
    ::close(socketDescriptor_);
  }
}

void NetworkAdapter::send(
    const int serverSocketDescriptor,
    const std::string& data) {
  int status = ::write(serverSocketDescriptor, data.c_str(), data.size());
  
  if(status < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
}

int NetworkAdapter::openIncommingConnectionSocket() {
  struct ::sockaddr_in clientAddress;
  std::size_t clientAddressSize = sizeof(clientAddress);
  int clientSocketDescriptor = ::accept(socketDescriptor_, reinterpret_cast<struct ::sockaddr*>(&clientAddress), reinterpret_cast<::socklen_t*>(&clientAddressSize));
  
  if(clientSocketDescriptor < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  
  return clientSocketDescriptor;
}

std::string NetworkAdapter::receive(
    const int clientSocketDescriptor) {
  std::array<char, 1024> buffer;
  int datasize = ::read(clientSocketDescriptor, buffer.data(), buffer.size());
  
  ::close(clientSocketDescriptor);
  
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
