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

arma::Col<double>::fixed<6> endEffectorPose = {0.0, 0.0, 0.24, 0.0, 0.0, 0.0};

int main(int argc, char **argv) {
  // Initializes WiringPi and uses the BCM pin layout.
  // For an overview on the pin layout, use the `gpio readall` command on a Raspberry Pi.
  ::wiringPiSetupGpio();

  demo::ExtensionSensors extensionSensors(demo::Gpio::allocateSpi(), {0, 1, 2, 3, 4, 5}, 0.168, 0.268);
  extensionSensors.setNumberOfSamplesPerMeasurment(3);
  arma::Mat<double> extensionSensorsCorrection;
  if (extensionSensorsCorrection.load("extensionSensors.correction")) {
    std::cout << "Using the extension sensor correction." << std::endl;
    extensionSensors.setMeasurementCorrections(extensionSensorsCorrection);
  } else {
    std::cout << "Could not find extension sensor correction file. Displaying uncorrected measurements." << std::endl;
  }

  std::vector<demo::Pin> directionPins;
  directionPins.push_back(demo::Gpio::allocatePin(22));
  directionPins.push_back(demo::Gpio::allocatePin( 5));
  directionPins.push_back(demo::Gpio::allocatePin( 6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  demo::ServoControllers servoControllers(std::move(directionPins), demo::Gpio::allocateI2c(), {0, 1, 2, 3, 4, 5}, 1.0);

  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors), 0.178, 0.248);
  linearActuators.setAcceptableExtensionDeviation(0.005);

  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart(), -arma::datum::pi, arma::datum::pi);

  arma::Mat<double>::fixed<3, 6> baseJointsPosition;
  baseJointsPosition.load("baseJointsPosition.config");
  arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
  endEffectorJointsRelativePosition.load("endEffectorJointsRelativePosition.config");

  demo::StewartPlatform stewartPlatform(std::move(linearActuators), std::move(attitudeSensors), baseJointsPosition, endEffectorJointsRelativePosition, {-0.02, -0.02, 0.21, -0.2, -0.2, -0.6}, {0.02, 0.02, 0.27, 0.2, 0.2, 0.6});

  demo::Network network(31415);

  std::string message = "";
  do {
    message = network.receive();

    if (message.substr(0, 3) == "get") {
      network.send("192.168.0.16", 31415, vectorToString(endEffectorPose));
    } else if (message.substr(0, 3) == "set") {
      message = message.substr(message.find(" ") + 1);
      stewartPlatform.setEndEffectorPose(stringToVector(message.substr(message.find(" ") + 1)));
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      network.send("192.168.0.16", 31415, "ACK");
    }
  } while (message != "exit");

  return EXIT_SUCCESS;
}
