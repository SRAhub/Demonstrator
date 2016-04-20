// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"
#include "../robot.hpp"

void showHelp();
void runCalibration();
arma::Cube<double> measure();

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

  runCalibration();

  return 0;
}

void runAll(
    demo::LinearActuators& linearActuators,
    double extension) {
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + extension, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
}

void runCalibration() {
  auto measurements = measure();
  // TODO: process measured values
  if (measurements.save("extension_sensor_adjustments.mat", arma::raw_ascii))
    std::cout << "Saved in `extension_sensor_adjustments.mat`; "
              << "each row represents one sensor.\n";
  else
    std::cout << "Saving of file `extension_sensor_adjustments.mat` failed.\n";
}

/**
 * Approach extension intervals from 10% to 80% inclusive, in 10% steps. To measure the behaviour of the motor when working both with and against the weight of the robot, all extension levels are reached twice, both by extending and retracting the linear actuators.
 *
 * Return a Cube<double>, where the slices represent a linear actuator, the columns an extension level, and the rows the measured values. The first index of a row contains samples obtained after extending the linear actuator, the second index samples obtained after retracting.
 *
 * The first and last extension levels are only approached from one direction because they are the limits for sane operation. Therefore the user is only asked to measure these distances once. The measured value is placed in both indices in the result.
 */
arma::Cube<double> measure() {
  auto linearActuators = createLinearActuators();
  linearActuators.getExtensionSensors().setNumberOfSamplesPerMeasurment(1);
  const std::array<double, 8> extensions {.1, .2, .3, .4, .5, .6, .7, .8};
  arma::Cube<double> result;

  std::cout << "Starting measurement\n";
  for (int i = 0; i < extensions.size(); i++) {
    std::cout << "Extending to " << extensions.at(i) << "%\n";
    runAll(linearActuators, extensions.at(i));
    std::cout << "Reached target extension.\n";
    for (std::size_t device = 0; device < linearActuators.numberOfActuators_; device++) {
      std::cout << "Enter measured distance for linear actuator #"
                << device
                << " (in percent): ";
      std::cin >> result(0, i, device);
      std::cout << std::endl;
    }

    if (i < extensions.size() - 1) {
      runAll(linearActuators, extensions.at(i + 1));
      std::cout << "Retracting to " << extensions.at(i) << "%\n";
      runAll(linearActuators, extensions.at(i));
      std::cout << "Reached target extension.\n";
      for (std::size_t device = 0; device < linearActuators.numberOfActuators_; device++) {
        std::cout << "Enter measured distance for linear actuator #"
                  << device
                  << " (in percent): ";
        std::cin >> result(1, i, device);
        std::cout << std::endl;
      }
    } else {
      std::cout << "Won't extend further than " << extensions.at(i) << "%. Copying first value.\n";
      for (std::size_t device = 0; device < linearActuators.numberOfActuators_; device++) {
        result(1, i, device) = result(0, i, device);
      }
    }
  }

  std::cout << "Done.\n";
  runAll(linearActuators, extensions.at(1));

  return result;
}
