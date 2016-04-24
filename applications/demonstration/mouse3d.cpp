// C++ standard library
#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <chrono>

// Demonstrator
#include <demonstrator>

int main (int argc, char **argv) {
  demo::Network network(31415);
  demo::Mouse3dSensors mouse3dSensors(-1, 1);
  mouse3dSensors.runAsynchronous();

  std::array<std::string, 4> motorPis = {{"192.168.0.1", "192.168.0.3", "192.168.0.5", "192.168.0.7"}};
  for (size_t n = 0; n < motorPis.size(); n++) {
    network.send(motorPis.at(n), 31415, "set 0 0 0.25 0 0 0");
  }

  arma::Row<double> data;

  do {
    network.send(motorPis.at(3), 31415, "get");
    std::string response = network.retrieve();
    for (size_t n = 0; n < 6; n++) {
      data.at(n) = std::stod(message.substr(0, message.find(" ")));
      message = message.substr(message.find(" ") + 1);
    }

    data = mouse3dSensors.measure();
    for (size_t n = 0; n < 6; n++) {
      if (std::abs(data.at(n)) < 0.5) {
        data += (data.at(n) / std::abs(data.at(n))) / 100;	// move 1cm
      }
    }

    for (size_t n = 0; n < motorPis.size(); n++) {
      network.send(motorPis.at(n), 31415, "");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  } while (data.at(6) == 0 || data.at(7) == 0);	// terminate program, when both mouse buttons are pressed

  return EXIT_SUCCESS;
}
