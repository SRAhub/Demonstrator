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
void runDefault(
    demo::ExtensionSensors&);

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

  runDefault(extensionSensors);

  return 0;
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Prints measurements for each sensor until the program is terminated\n";
  std::cout << "\n";
  std::cout << "  program calibrate [options ...]\n";
  std::cout << "    Starts the sensor calibration\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runDefault(
    demo::ExtensionSensors& extensionSensors) {

  while(1) {
    std::cout << "+--------------+--------------+--------------+--------------+--------------+--------------+\n"
              << "| Sensor 1 [m] | Sensor 2 [m] | Sensor 3 [m] | Sensor 4 [m] | Sensor 5 [m] | Sensor 6 [m] |\n"
              << "+--------------+--------------+--------------+--------------+--------------+--------------+" << std::endl;
    for (unsigned int n = 0; n < 10; ++n) {
      const arma::Row<double>& extensions = extensionSensors.measure();
      std::cout << "|";
      for (std::size_t k = 0; k < extensionSensors.numberOfSensors_; ++k) {
         std::cout << " " << std::setw(12) << extensions(k) << " |";
      }
      std::cout << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}
