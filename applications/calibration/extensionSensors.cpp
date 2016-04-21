// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runCalibration(
    demo::LinearActuators&);
arma::Mat<double> measure(
    demo::LinearActuators&);

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

  demo::ExtensionSensors extensionSensors(demo::Gpio::allocateSpi(), {0, 1, 2, 3, 4, 5}, 0.0, 1.0);

  std::vector<demo::Pin> directionPins;
  directionPins.push_back(demo::Gpio::allocatePin(22));
  directionPins.push_back(demo::Gpio::allocatePin(5));
  directionPins.push_back(demo::Gpio::allocatePin(6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  demo::ServoControllers servoControllers(std::move(directionPins), demo::Gpio::allocateI2c(), {0, 1, 2, 3, 4, 5}, 1.0);

  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors), 0.1, 0.8);
  linearActuators.setAcceptableExtensionDeviation(0.01);

  runCalibration(linearActuators);

  return 0;
}

void showHelp() {
  std::cout << "Usage:\n"
            << "  program [options ...]\n"
            << "    Moves all actuators up by 10%, then expects the user to measure and enter the true extension to calculate adjustments.\n"
            << "\n"
            << "  Options:\n"
            << "         --verbose    Prints additional (debug) information\n"
            << "    -h | --help       Displays this help\n"
            << std::flush;
}

void runAll(
    demo::LinearActuators& linearActuators,
    double extension) {
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + extension, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
  linearActuators.waitTillExtensionIsReached(std::chrono::microseconds({3 * 1000 * 1000}));
}

void runCalibration(
    demo::LinearActuators& linearActuators) {
  auto measurements = measure(linearActuators);
  // TODO: process measured values
  if (measurements.save("extensionSensors.calibration", arma::raw_ascii))
    std::cout << "Saved in `extensionSensors.calibration`; "
              << "each row represents one sensor.\n";
  else
    std::cout << "Saving of file `extensionSensors.calibration` failed.\n";
}

/**
 * Approach extension intervals from 10% to 80% inclusive, in 10% steps. To measure the behaviour of the motor when working both with and against the weight of the robot, all extension levels are reached twice, both by extending and retracting the linear actuators.
 *
 * Return a Mat<double> with values filled according to `model/README.md`.
 */
arma::Mat<double> measure(
    demo::LinearActuators& linearActuators) {
  linearActuators.getExtensionSensors().setNumberOfSamplesPerMeasurment(1);
  const std::array<double, 8> extensions {.1, .2, .3, .4, .5, .6, .7, .8};
  arma::Mat<double> result(linearActuators.numberOfActuators_, extensions.size());

  std::cout << "Starting measurement\n";
  for (int i = 0; i < extensions.size(); i++) {
    std::cout << "Extending to " << extensions.at(i) << "\n";
    runAll(linearActuators, extensions.at(i));
    std::cout << "Reached target extension.\n";
    for (std::size_t device = 0; device < linearActuators.numberOfActuators_; device++) {
      std::cout << "Enter measured distance for linear actuator #"
                << device
                << ": ";
      std::cin >> result(device, i);
    }

    if (i < extensions.size() - 1) {
      runAll(linearActuators, extensions.at(i + 1));
      std::cout << "Retracting to " << extensions.at(i) << "\n";
      runAll(linearActuators, extensions.at(i));
      std::cout << "Reached target extension.\n";
      for (std::size_t device = 0; device < linearActuators.numberOfActuators_; device++) {
        std::cout << "Enter measured distance for linear actuator #"
                  << device
                  << ": ";
        double value;
        std::cin >> value;
        result(device, i) = (value + result(device, i)) / 2;
      }
    } else {
      std::cout << "Won't extend further than " << extensions.at(i) << std::endl;
    }
  }

  std::cout << "Done.\n";
  runAll(linearActuators, extensions.at(1));

  return result;
}
