#pragma once

// C++ Standard Library
#include <string>
#include <map>

namespace demo {
  class NetworkAdapter {
   public:
    explicit NetworkAdapter(
        const std::string& hostname,
        const unsigned int port);
    ~NetworkAdapter();


    std::string receive(
        //const std::string& hostname
        const int fd);
    void send(
        //const std::string& hostname,
        const int fd,
        const std::string& data);



   protected:
    int openIncommingConnectionSocket(
        //const std::string& hostname
        );
    int establishOutgoingConnectionSocket(
        const std::string& hostname,
        const unsigned int port);

    //int incommingSocketDescriptor_;
    //std::map<std::string, int> outgoingSocketDescriptors_;
    std::string hostname_;
  };
}
