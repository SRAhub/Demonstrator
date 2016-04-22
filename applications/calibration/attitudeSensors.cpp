// C++ standard library
#include <chrono>
#include <iomanip>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runCalibration(
    demo::StewartPlatform& stewartPlatform);
void runReset(
    demo::AttitudeSensors& attitudeSensors,
    demo::LinearActuators& linearActuators);

int main (const int argc, const char* argv[]) {
  if (hasOption(argc, argv, "-h") || hasOption(argc, argv, "--help")) {
    showHelp();
    // Terminates the program after the help is shown.
    return 0;
  }

  if (hasOption(argc, argv, "--verbose")) {
    ::demo::isVerbose = true;
  }

  // Initialises WiringPi and uses the BCM pin layout.
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
  directionPins.push_back(demo::Gpio::allocatePin(5));
  directionPins.push_back(demo::Gpio::allocatePin(6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  demo::ServoControllers servoControllers(std::move(directionPins), demo::Gpio::allocateI2c(), {0, 1, 2, 3, 4, 5}, 1.0);
  
  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors), 0.178, 0.248);
  linearActuators.setAcceptableExtensionDeviation(0.0005);
  
  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart(), -arma::datum::pi, arma::datum::pi);

  if (hasOption(argc, argv, "reset")) {
    runReset(attitudeSensors, linearActuators);
  } else {
    arma::Mat<double>::fixed<3, 6> baseJointsPosition;
    baseJointsPosition.load("baseJointsPosition.config");
    arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
    endEffectorJointsRelativePosition.load("endEffectorJointsRelativePosition.config");
    
    demo::StewartPlatform stewartPlatform(std::move(linearActuators), std::move(attitudeSensors), baseJointsPosition, endEffectorJointsRelativePosition);
    
    runCalibration(stewartPlatform);
  }
  
  return 0;
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Starts the sensor calibration\n";
  std::cout << "\n";
  std::cout << "  program reset [options ...]\n";
  std::cout << "    Sets the current attitudes as (0, 0, 0).\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush; 
}

void runCalibration(
    demo::StewartPlatform& stewartPlatform) {
  std::size_t numberOfSamplesPerMeasurement;
  std::cout << "Number of samples per measurement: ";
  std::cin >> numberOfSamplesPerMeasurement;

  std::cout << "Approaching the initial position (0.0, 0.0, 0.25, 0.0, 0.0, 0.0)." << std::endl;
  const arma::Col<double> initialEndEffectorPose = {0.0, 0.0, 0.25, 0.0, 0.0, 0.0};
  stewartPlatform.setEndEffectorPose(initialEndEffectorPose);
  stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));

  std::cout << "Starting attitude sensor calibration ..." << std::endl;
  const arma::Mat<double>::fixed<7, 3>& expectedMeasuredAttitudes = {
    -0.261799, -0.174533, -0.0872665, 0.0, 0.0872665, 0.174533, 0.261799,
    -0.261799, -0.174533, -0.0872665, 0.0, 0.0872665, 0.174533, 0.261799,
    -0.261799, -0.174533, -0.0872665, 0.0, 0.0872665, 0.174533, 0.261799
  };
  arma::Cube<double> actualMeasuredAttitudes(numberOfSamplesPerMeasurement, expectedMeasuredAttitudes.n_rows, expectedMeasuredAttitudes.n_cols);
  
  for (std::size_t n = 0; n < expectedMeasuredAttitudes.n_cols; ++n) {
    std::cout << "- Calibrating sensor " << n << std::endl;
    arma::Col<double> endEffectorPose = initialEndEffectorPose;
    stewartPlatform.setEndEffectorPose(initialEndEffectorPose);
    stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    for (std::size_t k = 0; k < expectedMeasuredAttitudes.n_rows; ++k) {
      std::cout << "- Approaching " << expectedMeasuredAttitudes(k, n) << " radians" << std::endl;
      endEffectorPose(3 + n) = expectedMeasuredAttitudes(k, n);
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      for (std::size_t l = 0; l < actualMeasuredAttitudes.n_rows; ++l) {
        try {
          const arma::Col<double>::fixed<6>& measuredEndEffectorPose = stewartPlatform.getEndEffectorPose();
          actualMeasuredAttitudes.subcube(l, k, n, l, k, n) = measuredEndEffectorPose(3 + n);
        } catch (...) {
          actualMeasuredAttitudes.subcube(l, k, n, l, k, n) = arma::datum::nan;
        }
      }
    }
  }
  std::cout << "... Done." << std::endl;
  stewartPlatform.setEndEffectorPose(initialEndEffectorPose);
  stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
  
  for (std::size_t n = 0; n < 3; ++n) {
    static_cast<arma::Mat<double>>(actualMeasuredAttitudes.slice(n)).save("actualMeasuredAttitudes_sensor" + std::to_string(n) + ".mat", arma::raw_ascii);
  }
  for (std::size_t n = 0; n < expectedMeasuredAttitudes.n_rows; ++n) {
    static_cast<arma::Row<double>>(expectedMeasuredAttitudes.row(n)).save("expectedMeasuredAttitudes_sensor" + std::to_string(n) + ".mat", arma::raw_ascii);
  }
}

void runReset(
    demo::AttitudeSensors& attitudeSensors,
    demo::LinearActuators& linearActuators) {
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + linearActuators.minimalAllowedExtension_, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
      
  attitudeSensors.runAsynchronous();
  attitudeSensors.reset();
}
