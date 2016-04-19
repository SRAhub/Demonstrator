// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "commandline.hpp"

void showHelp();
void runDefault(
    demo::LinearActuators& linearActuators);
void runAll(
    demo::LinearActuators& linearActuators,
    const double extension);
void runSingle(
    demo::LinearActuators& linearActuators,
    const std::size_t n,
    const double extension);
void runCalibration(
    demo::LinearActuators& linearActuators);
arma::Cube<double> measure(
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

  std::vector<demo::Pin> directionPins;
  directionPins.push_back(demo::Gpio::allocatePin(22));
  directionPins.push_back(demo::Gpio::allocatePin(5));
  directionPins.push_back(demo::Gpio::allocatePin(6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  demo::I2c i2c = demo::Gpio::allocateI2c();
  std::vector<unsigned int> i2cChannels = {0, 1, 2, 3, 4, 5};
  demo::ServoControllers servoControllers(std::move(directionPins), std::move(i2c), i2cChannels);
  servoControllers.setMaximalSpeed(1.0);

  demo::Spi spi = demo::Gpio::allocateSpi();
  std::vector<unsigned int> spiChannels = {0, 1, 2, 3, 4, 5};
  demo::ExtensionSensors extensionSensors(std::move(spi), spiChannels);
  extensionSensors.setMinimalMeasurableValue(0.1);
  extensionSensors.setMaximalMeasurableValue(1.0);

  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors));
  linearActuators.setMinimalExtension(0.1);
  linearActuators.setMaximalExtension(0.8);
  linearActuators.setMaximalExtensionDeviation(0.05);
  if (hasOption(argc, argv, "calibrate")) {
    runCalibration(linearActuators);
  } else if (argc > 2 && argv[1][0] != '-' && argv[2][0] != '-') {
    runSingle(linearActuators, std::stoi(argv[1]), std::stod(argv[2]));
  } else if (argc > 1 && argv[1][0] != '-') {
    runAll(linearActuators, std::stod(argv[1]));
  } else {
    runDefault(linearActuators);
  }

  return 0;
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Moves one actuator after another up by 15% and then down again\n";
  std::cout << "\n";
  std::cout << "  program calibrate [options ...]\n";
  std::cout << "    Starts the sensor calibration\n";
  std::cout << "\n";
  std::cout << "  program extension [options ...]\n";
  std::cout << "    Moves all actuators to `extension`\n";
  std::cout << "\n";
  std::cout << "  program n extension [options ...]\n";
  std::cout << "    Moves the `n`-th actuator to `extension`\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runDefault(
    demo::LinearActuators& linearActuators) {
  arma::Row<double> extensions = linearActuators.getExtensions();
  const arma::Row<double> maximalSpeeds(linearActuators.numberOfActuators_, arma::fill::ones);

  while(1) {
    for (std::size_t n = 0; n < linearActuators.numberOfActuators_; ++n) {
      std::cout << "Moving actuator " << n << " down by 15%." << std::endl;

      extensions(n) -= 0.15;
      linearActuators.setExtensions(extensions, maximalSpeeds);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (std::size_t n = 0; n < linearActuators.numberOfActuators_; ++n) {
      std::cout << "Moving actuator " << n << " up by 15%." << std::endl;

      extensions(n) += 0.15;
      linearActuators.setExtensions(extensions, maximalSpeeds);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void runAll(
    demo::LinearActuators& linearActuators,
    double extension) {
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + extension, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
}

void runSingle(
    demo::LinearActuators& linearActuators,
    const std::size_t n,
    const double extension) {
  arma::Row<double> extensions = linearActuators.getExtensions();
  extensions(n) = extension;
  arma::Row<double> maximalSpeeds(linearActuators.numberOfActuators_, arma::fill::zeros);
  maximalSpeeds(n) = 1.0;

  linearActuators.setExtensions(extensions, maximalSpeeds);
}

void runCalibration(
    demo::LinearActuators& linearActuators) {
  auto measurements = measure(linearActuators);
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
arma::Cube<double> measure(
    demo::LinearActuators& linearActuators) {
  const std::array<double, 8> extensions {.1, .2, .3, .4, .5, .6, .7, .8};
  linearActuators.getExtensionSensors().setNumberOfSamplesPerMeasurment(1);
  arma::Cube<double> result;

  std::cout << "Starting measurement\n";
  for (int i = 0; i < extensions.size(); i++) {
    if (i != 0) {
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
    }
    if (i != extensions.size() - 1) {
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
    }
  }
  for (int device = 0; device < linearActuators.numberOfActuators_; device++) {
    result(0, 0, device) = result(1, 0, device);
    result(1, extensions.size() - 1, device) = result(0, extensions.size() - 1, device);
  }

  std::cout << "Done.\n";
  runAll(linearActuators, extensions.at(1));

  return result;
}
