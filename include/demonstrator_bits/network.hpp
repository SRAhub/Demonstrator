#pragma once

// C++ Standard Library
#include <string>
#include <map>

namespace demo {
  class NetworkAdapter {
   public:
    NetworkAdapter() = delete;
    NetworkAdapter(NetworkAdapter&) = delete;
    NetworkAdapter operator=(NetworkAdapter&) = delete;
    explicit NetworkAdapter(
        const unsigned int port);
    ~NetworkAdapter();


    std::string receive(
        const std::string& hostname);
    void send(
        const std::string& hostname,
        const std::string& data);



   protected:
    int openIncommingConnectionSocket();
    int establishOutgoingConnectionSocket(
        const std::string& hostname,
        const unsigned int port);

    int incomingSocketDescriptor_;
  };
}
