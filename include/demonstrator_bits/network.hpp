#pragma once

// C++ Standard Library
#include <string>
#include <map>

namespace demo {
  class NetworkAdapter {
   public:
    explicit NetworkAdapter(
        const unsigned int port);

    // Functions for receiving data
    int openIncommingConnectionSocket(
        const std::string& hostname);
    std::string receive(
        const std::string& hostname);

    // Functions for sending data
    int establishOutgoingConnectionSocket(
        const std::string& hostname,
        const unsigned int port);
    void send(
        const std::string& hostname,
        const std::string& data);

   protected:
    int incommingSocketDescriptor_;
    std::map<std::string, int> outgoingSocketDescriptors_;
  };
}
