// C++ standard library
#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <chrono>
#include <queue>

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
    network.receive(); // ACK
  }
  
  std::queue<arma::Row<double>::fixed<6>> fixedMovement;
  fixedMovement.push({0.0, 0.0, 0.8, 0.4, 0.0, 0.0});
  fixedMovement.push({0.0, 0.0, 0.84, 0.0, 0.0, -0.12});
  fixedMovement.push({0.0, 0.0, 0.88, 0.4, 0.0, 0.0});
  fixedMovement.push({0.0, 0.0, 0.92, 0.0, 0.0, 0.12});
  fixedMovement.push({0.0, 0.0, 0.96, 0.4, 0.0, 0.0});
  fixedMovement.push({0.0, 0.0, 1.0, 0.0, 0.4, 0.0});
  fixedMovement.push({0.0, 0.0, 1.04, 0.4, 0.0, 0.0});

  while(1) {
    endEffectorPose = fixedMovement.front();
    fixedMovement.pop();
    fixedMovement.push(endEffectorPose);

    for (size_t n = 0; n < motorPis.size(); n++) {
      network.send(motorPis.at(n), 31415, vectorToString(endEffectorPose / motorPis.size()));
      network.receive(); // ACK
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  };

  return EXIT_SUCCESS;
}
