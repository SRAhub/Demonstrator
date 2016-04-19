#include "robot.hpp"

// C++ standard library
#include <vector>

demo::Network&& createNetwork() {
  return std::move(demo::Network(9001));
}

demo::AttitudeSensors&& createAttitudeSensors() {
  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart(), -arma::datum::pi, arma::datum::pi);
  attitudeSensors.runAsynchronous();
  
  return std::move(attitudeSensors);
}

demo::DistanceSensors&& createDistanceSensors() {
  std::vector<demo::Pin> pins;
  pins.push_back(demo::Gpio::allocatePin(17));
  pins.push_back(demo::Gpio::allocatePin(27));
  pins.push_back(demo::Gpio::allocatePin(22));
  pins.push_back(demo::Gpio::allocatePin(10));
  pins.push_back(demo::Gpio::allocatePin(25));
  pins.push_back(demo::Gpio::allocatePin(11));
  
  return std::move(demo::DistanceSensors(std::move(pins), 0.03, 0.35));
}

demo::ExtensionSensors&& createExtensionSensors() {
  return std::move(demo::ExtensionSensors(demo::Gpio::allocateSpi(), {0, 1, 2, 3, 4, 5}, 0.0, 1.0));
}

demo::Mouse3dSensors&& createMouse3dSensors() {
  return std::move(demo::Mouse3dSensors(-1.0, 1.0));
}

demo::DistanceIndicators&& createDistanceIndicators() {
  std::vector<demo::Pin> dataPins;
  dataPins.push_back(demo::Gpio::allocatePin(12));
  dataPins.push_back(demo::Gpio::allocatePin(5));
  dataPins.push_back(demo::Gpio::allocatePin(6));
  dataPins.push_back(demo::Gpio::allocatePin(13));
  dataPins.push_back(demo::Gpio::allocatePin(19));
  dataPins.push_back(demo::Gpio::allocatePin(26));
  
  return std::move(demo::DistanceIndicators(demo::Gpio::allocatePin(21), std::move(dataPins), 0.05, 0.08, 0.20));
}

demo::ServoControllers&& createServoControllers() {
  std::vector<demo::Pin> directionPins;
  directionPins.push_back(demo::Gpio::allocatePin(22));
  directionPins.push_back(demo::Gpio::allocatePin(5));
  directionPins.push_back(demo::Gpio::allocatePin(6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  
  return std::move(demo::ServoControllers(std::move(directionPins), demo::Gpio::allocateI2c(), {0, 1, 2, 3, 4, 5}, 1.0));
}

demo::LinearActuators&& createLinearActuators() {
  demo::LinearActuators linearActuators(createServoControllers(), createExtensionSensors(), 0.1, 0.8);
  linearActuators.setMaximalExtensionDeviation(0.05);
  
  return std::move(linearActuators);
}

demo::StewartPlatform&& createStewartPlatform() {
  arma::Mat<double>::fixed<3, 6> baseJointsPosition;
  baseJointsPosition.load("baseJointsPosition.mat");
  arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
  endEffectorJointsRelativePosition.load("endEffectorJointsRelativePosition.mat");
  arma::Row<double>::fixed<6> actuatorsMinimalLength;
  actuatorsMinimalLength.load("actuatorsMinimalLength.mat");
  arma::Row<double>::fixed<6> actuatorsMaximalLength;
  actuatorsMaximalLength.load("actuatorsMaximalLength.mat");
  
  return std::move(demo::StewartPlatform(createLinearActuators(), createAttitudeSensors(), baseJointsPosition, endEffectorJointsRelativePosition, actuatorsMinimalLength, actuatorsMaximalLength));
}

