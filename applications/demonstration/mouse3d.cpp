// C++ standard library
#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <chrono>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

int main (int argc, char **argv) {
  demo::Network network(31415);
  demo::Mouse3dSensors mouse3dSensors(-1, 1);
  mouse3dSensors.runAsynchronous();

  std::array<std::string, 4> motorPis = {{"192.168.0.1", "192.168.0.3", "192.168.0.5", "192.168.0.7"}};
  for (size_t n = 0; n < motorPis.size(); n++) {
    network.send(motorPis.at(n), 31415, "set 0 0 0.25 0 0 0");
  }
  std::this_thread::sleep_for(std::chrono::seconds(10));

  while(1) {
    network.send(motorPis.at(3), 31415, "get");
    arma::Row<double>::fixed<6> endEffectorPose = stringToVector(network.receive());

    arma::Row<double>::fixed<8> mouse3d = mouse3dSensors.measure();    
    for (size_t n = 0; n < 3; n++) {
      if (std::abs(mouse3d(n)) > 0.9) {
        endEffectorPose(n) += std::copysign(0.005, -mouse3d(n));
      }
    }

    for (size_t n = 0; n < motorPis.size(); n++) {
      network.send(motorPis.at(n), 31415, "set " + vectorToString(endEffectorPose));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  };

  return EXIT_SUCCESS;
}
