// C++ standard library
#include <string>
#include <iostream>
#include <atomic>
#include <thread>

// Wiring Pi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void networkControl();

std::atomic<bool> running;
arma::Row<double> distances;

int main(int argc, char **argv) {
  // Initializes WiringPi and uses the BCM pin layout.
  // For an overview on the pin layout, use the `gpio readall` command on a Raspberry Pi.
  ::wiringPiSetupGpio();

  std::vector<demo::Pin> sensorPins;
  sensorPins.push_back(demo::Gpio::allocatePin(17));
  sensorPins.push_back(demo::Gpio::allocatePin(27));
  sensorPins.push_back(demo::Gpio::allocatePin(22));
  sensorPins.push_back(demo::Gpio::allocatePin(10));
  sensorPins.push_back(demo::Gpio::allocatePin(25));
  sensorPins.push_back(demo::Gpio::allocatePin(11));
  demo::DistanceSensors distanceSensors(std::move(sensorPins), 0.03, 0.35);
  distanceSensors.setNumberOfSamplesPerMeasurment(3);

  std::vector<demo::Pin> dataPins;
  dataPins.push_back(demo::Gpio::allocatePin(12));
  dataPins.push_back(demo::Gpio::allocatePin( 5));
  dataPins.push_back(demo::Gpio::allocatePin( 6));
  dataPins.push_back(demo::Gpio::allocatePin(13));
  dataPins.push_back(demo::Gpio::allocatePin(19));
  dataPins.push_back(demo::Gpio::allocatePin(26));
  demo::DistanceIndicators distanceIndicators(demo::Gpio::allocatePin(21), std::move(dataPins), 0.05, 0.08, 0.20);

  running = true;
  std::thread networkThread(networkControl);

  while (running) {
    distances = distanceSensors.measure();
    distanceIndicators.setIndication(distances);
  }

  networkThread.join();
  return EXIT_SUCCESS;
}

void networkControl() {
  std::string message = "";
  demo::Network network(31415);

  do {
    message = network.receive();

    if (message.substr(0, 3) == "get") {
      network.send("192.168.0.16", 31415, vectorToString(distances));
    }
  } while (message != "exit" && running);

  running = false;
}
