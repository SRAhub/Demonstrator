#pragma once

// C++ Standard Library
#include <string>

namespace demo {
  class NetworkAdapter {
   public:
    explicit NetworkAdapter(
        const unsigned int port);

    std::string receive(
        const int fileDescriptor);

    void send(
        const std::string& hostname,
        const std::string& data);
  };
}
