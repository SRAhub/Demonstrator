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
void runDefault(
    demo::LinearActuators& linearActuators);
void runAll(
    demo::LinearActuators& linearActuators,
    const double extension);
void runSingle(
    demo::LinearActuators& linearActuators,
    const std::size_t n,
    const double extension);
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
  linearActuators.setAcceptableExtensionDeviation(0.001);
  
  if (argc > 2 && isNumber(argv[1]) && isNumber(argv[2])) {
    runSingle(linearActuators, std::stoi(argv[1]), std::stod(argv[2]));
  } else if (argc > 1 && isNumber(argv[1])) {
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
      linearActuators.waitTillExtensionIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::seconds(1));
      
      std::cout << "Moving actuator " << n << " up by 15%." << std::endl;

      extensions(n) += 0.15;
      linearActuators.setExtensions(extensions, maximalSpeeds);
      linearActuators.waitTillExtensionIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void runAll(
    demo::LinearActuators& linearActuators,
    double extension) {
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + extension, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
  linearActuators.waitTillExtensionIsReached(std::chrono::seconds(10));
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
  linearActuators.waitTillExtensionIsReached(std::chrono::seconds(10));
}
