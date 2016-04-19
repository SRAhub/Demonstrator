// C++ standard library
#include <iostream>

// Demonstrator
#include "robot.hpp";

arma::Mat<double> loadOrExit(
    char* filename) {
  arma::Mat<double> matrix;
  if (matrix.load(filename, raw_ascii)) {
    return matrix;
  }
  std::cerr << "Couldn't load file " << filename << std::flush;
  exit(1);
}

demo::ServoControllers createServoControllers() {
  std::vector<demo::Pin> directionPins;
  directionPins.push_back(demo::Gpio::allocatePin(22));
  directionPins.push_back(demo::Gpio::allocatePin(5));
  directionPins.push_back(demo::Gpio::allocatePin(6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  demo::ServoControllers servoControllers(
      std::move(directionPins),
      demo::Gpio::allocateI2c(),
      {0, 1, 2, 3, 4, 5});
  servoControllers.setMaximalSpeed(1.0);
  return servoControllers;
}

demo::ExtensionSensors createExtensionSensors(
    bool readCalibrationFiles) {
  demo::ExtensionSensors extensionSensors(
      demo::Gpio::allocateSpi(),
      {0, 1, 2, 3, 4, 5});
  extensionSensors.setMinimalMeasurableValue(0.1);
  extensionSensors.setMaximalMeasurableValue(1.0);
  if (readCalibrationFiles) {
    extensionSensors.setMeasurementCorrections(loadOrExit("model/extensionSensors.calibration"));
  }
  return extensionSensors;
}

demo::LinearActuators createLinearActuators(
    bool readCalibrationFiles) {
  demo::LinearActuators linearActuators(
      createServoControllers(),
      createExtensionSensors());
  linearActuators.setMinimalExtension(0.1);
  linearActuators.setMaximalExtension(0.8);
  linearActuators.setMaximalExtensionDeviation(0.05);
  return linearActuators;
}

demo::AttitudeSensors createAttitudeSensors(
    bool readCalibrationFiles) {
  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart());
  if (readCalibrationFiles) {
    attitudeSensors.setMeasurementCorrections(loadOrExit("model/extensionSensors.calibration"));
  }
  attitudeSensors.runAsynchronous();
  return attitudeSensors;
}

demo::StewartPlatform createStewartPlatform(bool readCalibrationFiles) {
  arma::Mat<double>::fixed<3, 6> baseJointsPosition;
  arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
  arma::Row<double>::fixed<6> actuatorsMinimalLength;
  arma::Row<double>::fixed<6> actuatorsMaximalLength;

  loadOrExit(baseJointsPosition, "model/baseJointsPosition.config");
  loadOrExit(endEffectorJointsRelativePosition, "model/endEffectorJointsRelativePosition.config");
  loadOrExit(actuatorsMinimalLength, "model/actuatorsMinimalLength.config");
  loadOrExit(actuatorsMaximalLength, "model/actuatorsMaximalLength.config");

  StewartPlatform stewartPlatform(
      createLinearActuators(),
      createAttitudeSensors(),
      baseJointsPosition,
      endEffectorJointsRelativePosition,
      actuatorsMinimalLength,
      actuatorsMaximalLength);

  attitudeSensors.setMinimalMeasurableValue(-arma::datum::pi);
  attitudeSensors.setMaximalMeasurableValue(arma::datum::pi);
}
