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

  arma::Row<double>::fixed<6> endEffectorPose = {0.0, 0.0, 0.25, 0.0, 0.0, 0.0};
  
  std::array<std::string, 4> motorPis = {{"192.168.0.1", "192.168.0.3", "192.168.0.5", "192.168.0.7"}};
  for (size_t n = 0; n < motorPis.size(); n++) {
    network.send(motorPis.at(n), 31415, "set 0 0 0.25 0 0 0");
  }

  std::queue<arma::Row<double>::fixed<6>> fixedMovement;  
  
  fixedMovement.push(endEffectorPose);
  for (unsigned int n = 0; n < 2; ++n) {
    endEffectorPose(n) -= 0.01;
    fixedMovement.push(endEffectorPose);
    
    endEffectorPose(n) += 0.02;
    fixedMovement.push(endEffectorPose);
    
    endEffectorPose(n) = 0.0;
    fixedMovement.push(endEffectorPose);
  }
  
  for (unsigned int n = 3; n < 5; ++n) {
    endEffectorPose(n) -= 0.07;
    fixedMovement.push(endEffectorPose);
    
    endEffectorPose(n) += 0.14;
    fixedMovement.push(endEffectorPose);
    
    endEffectorPose(n) = 0.0;
    fixedMovement.push(endEffectorPose);
  }

  while(1) {
    for (size_t n = 0; n < motorPis.size(); n++) {
      endEffectorPose = fixedMovement.front();
      network.send(motorPis.at(n), 31415, "set " + vectorToString(endEffectorPose));
      fixedMovement.pop();
      fixedMovement.push(endEffectorPose);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  };

  return EXIT_SUCCESS;
}
