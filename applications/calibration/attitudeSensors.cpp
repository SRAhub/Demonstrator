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
    demo::AttitudeSensors&);
void runReset(
    demo::AttitudeSensors&);

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

  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart(), -arma::datum::pi, arma::datum::pi);

  if (hasOption(argc, argv, "reset")) {
    runReset(attitudeSensors);
  } else {
    runCalibration(attitudeSensors);
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
    demo::AttitudeSensors& attitudeSensors) {
  attitudeSensors.runAsynchronous();
  
  
}

void runReset(
    demo::AttitudeSensors& attitudeSensors) {
  attitudeSensors.runAsynchronous();
  attitudeSensors.reset();
}
