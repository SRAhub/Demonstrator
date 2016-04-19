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
#include "../robot.hpp"

bool useDistanceIndicators = false;

void showHelp();
void runDefault();

int main (const int argc, const char* argv[]) {
  if (hasOption(argc, argv, "-h") || hasOption(argc, argv, "--help")) {
    showHelp();
    // Terminates the program after the help is shown.
    return 0;
  }
  
  if (hasOption(argc, argv, "--indicators")) {
    ::useDistanceIndicators = true;
  }
  
  if (hasOption(argc, argv, "--verbose")) {
    ::demo::isVerbose = true;
  }
  
  // Initialises WiringPi and uses the BCM pin layout.
  // For an overview on the pin layout, use the `gpio readall` command on a Raspberry Pi.
  ::wiringPiSetupGpio();
  
  runDefault();
  
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
  std::cout << "  program evasion [options ...]\n";
  std::cout << "    Sends the measured distances to the motor Pi\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --indicators    Uses the distance indicators as additional output devices\n";
  std::cout << "         --verbose       Prints additional (debug) information\n";
  std::cout << "    -h | --help          Displays this help\n";
  std::cout << std::flush;
}

void runDefault() {
  demo::DistanceSensors distanceSensors(std::move(createDistanceSensors()));
  demo::DistanceIndicators distanceIndicators(std::move(createDistanceIndicators()));
  
  while(1) {
    std::cout << "+--------------+--------------+--------------+--------------+--------------+--------------+\n"
              << "| Sensor 1 [m] | Sensor 2 [m] | Sensor 3 [m] | Sensor 4 [m] | Sensor 5 [m] | Sensor 6 [m] |\n"
              << "+--------------+--------------+--------------+--------------+--------------+--------------+" << std::endl;
    for (unsigned int n = 0; n < 10; ++n) {
      const arma::Row<double>& distances = distanceSensors.measure();
      std::cout << "|";
      for (std::size_t k = 0; k < distanceSensors.numberOfSensors_; ++k) {
         std::cout << " " << std::setw(12) << distances(k) << " |";
      }
      std::cout << std::endl;
      if (::useDistanceIndicators) {
        distanceIndicators.setIndication(distances);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}
