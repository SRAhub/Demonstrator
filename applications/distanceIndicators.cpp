// C++ standard library
#include <chrono>
#include <iomanip>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "commandline.hpp"

void showHelp();
void runDefault(
    demo::DistanceIndicators&& distanceIndicators);
void runSensor(
    demo::DistanceIndicators&& distanceIndicators,
    demo::DistanceSensors&& distanceSensors);

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
  
  demo::Pin clockPin = demo::Gpio::allocatePin(21);
  std::vector<demo::Pin> dataPins;
  dataPins.push_back(demo::Gpio::allocatePin(12));
  dataPins.push_back(demo::Gpio::allocatePin(5));
  dataPins.push_back(demo::Gpio::allocatePin(6));
  dataPins.push_back(demo::Gpio::allocatePin(13));
  dataPins.push_back(demo::Gpio::allocatePin(19));
  dataPins.push_back(demo::Gpio::allocatePin(26));
  
  demo::DistanceIndicators distanceIndicators(std::move(clockPin), std::move(dataPins));
  distanceIndicators.setMinimalDistance(0.05); 
  distanceIndicators.setWarningDistance(0.08);
  distanceIndicators.setMaximalDistance(0.20);
  
  std::vector<demo::Pin> pins;
  pins.push_back(demo::Gpio::allocatePin(17));
  pins.push_back(demo::Gpio::allocatePin(27));
  pins.push_back(demo::Gpio::allocatePin(22));
  pins.push_back(demo::Gpio::allocatePin(10));
  pins.push_back(demo::Gpio::allocatePin(25));
  pins.push_back(demo::Gpio::allocatePin(11));
  demo::DistanceSensors distanceSensors(std::move(pins));
  distanceSensors.setMinimalMeasurableValue(0.03); 
  distanceSensors.setMaximalMeasurableValue(0.35);
  
  if (hasOption(argc, argv, "sensor")) {
    runSensor(std::move(distanceIndicators), std::move(distanceSensors));
  } else {
    runDefault(std::move(distanceIndicators));
  }
  
  return 0;  
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Runs from the minimal to the maximal distance\n";
  std::cout << "\n";
  std::cout << "  program sensor [options ...]\n";
  std::cout << "    Uses the distance sensor as input\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runDefault(
    demo::DistanceIndicators&& distanceIndicators) {
  while(1) {
    for (const auto distance : arma::linspace(distanceIndicators.getMaximalDistance(), distanceIndicators.getWarningDistance(), 9)) {
      std::cout << "Distance = " << distance << "m" << std::endl;
      distanceIndicators.setIndication(arma::zeros<arma::Row<double>>(distanceIndicators.numberOfIndicators_) + distance);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Distance = " << distanceIndicators.getMinimalDistance() << "m" << std::endl;
    distanceIndicators.setIndication(arma::zeros<arma::Row<double>>(distanceIndicators.numberOfIndicators_) + distanceIndicators.getMinimalDistance());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void runSensor(
    demo::DistanceIndicators&& distanceIndicators,
    demo::DistanceSensors&& distanceSensors) {
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
      distanceIndicators.setIndication(distances);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}
