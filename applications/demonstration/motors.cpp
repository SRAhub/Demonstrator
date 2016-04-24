// C++ standard library
#include <string>
#include <iostream>
#include <atomic>
#include <thread>

// Wiring Pi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

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
  linearActuators.setAcceptableExtensionDeviation(0.001);

  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart(), -arma::datum::pi, arma::datum::pi);

  arma::Mat<double>::fixed<3, 6> baseJointsPosition;
  baseJointsPosition.load("baseJointsPosition.config");
  arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
  endEffectorJointsRelativePosition.load("endEffectorJointsRelativePosition.config");

  demo::StewartPlatform stewartPlatform(std::move(linearActuators), std::move(attitudeSensors), baseJointsPosition, endEffectorJointsRelativePosition, {0.21, -0.1, -0.1, -0.261799, -0.261799, -0.610865}, {0.28, 0.1, 0.1, 0.261799, 0.261799, 0.610865});

  demo::Network network(31415);

  std::string message = "";
  do {
    message = network.receive();

    if (message == "get") {
      std::string response = "";
      arma::Col<double>::fixed<6> pose = stewartPlatform.getEndEffectorPose();
      for (size_t n = 0; n < pose.size(); n++) {
        response += std::to_string(pose.at(n)) + (n < pose.size() ? " " : "");
      }
      network.send("192.168.0.16", 31415, response);

    } else if (message == "set") {
      message = message.substr(message.find(" ") + 1);
      
      arma::Col<double>::fixed<6> pose;
      for (size_t n = 0; n < 6; n++) {
        pose.at(n) = std::stod(message.substr(0, message.find(" ")));
        message = message.substr(message.find(" ") + 1);
      }
      stewartPlatform.setEndEffectorPose(pose);
    }
  } while (message != "exit");

  return EXIT_SUCCESS;
}
