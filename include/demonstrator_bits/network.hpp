#pragma once

// C++ Standard Library
#include <string>

namespace demo {
  class Network {
   public:
    explicit Network(
        const unsigned int port_);

    explicit Network(
        Network&& network);

    Network& operator=(
        Network&& network);

    Network(Network&) = delete;
    Network& operator=(Network&) = delete;

    ~Network();

    void send(
        const std::string& hostname,
        const unsigned int port,
        const std::string& data);

    std::string receive();

   protected:
    int queuedSocketDescriptors_;
  };
}
