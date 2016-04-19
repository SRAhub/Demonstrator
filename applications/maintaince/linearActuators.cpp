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
void runDefault();
void runAll(
    const double extension);
void runSingle(
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

  if (argc > 2 && argv[1][0] != '-' && argv[2][0] != '-') {
    runSingle(std::stoi(argv[1]), std::stod(argv[2]));
  } else if (argc > 1 && argv[1][0] != '-') {
    runAll(std::stod(argv[1]));
  } else {
    runDefault();
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

void runDefault() {
  demo::LinearActuators linearActuators(std::move(createLinearActuators()));
  
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
    double extension) {
  demo::LinearActuators linearActuators(std::move(createLinearActuators()));
    
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + extension, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
}

void runSingle(
    const std::size_t n,
    const double extension) {
  demo::LinearActuators linearActuators(std::move(createLinearActuators()));
  
  arma::Row<double> extensions = linearActuators.getExtensions();
  extensions(n) = extension;
  arma::Row<double> maximalSpeeds(linearActuators.numberOfActuators_, arma::fill::zeros);
  maximalSpeeds(n) = 1.0;

  linearActuators.setExtensions(extensions, maximalSpeeds);
}
