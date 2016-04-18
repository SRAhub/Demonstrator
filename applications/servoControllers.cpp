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
void run(
    demo::ServoControllers& servoControllers,
    const std::size_t n);

int main (const int argc, const char* argv[]) {
  if (argc < 2) {
    showHelp();
    return 1;
  }
  
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
  std::vector<unsigned int> channels = {0, 1, 2, 3, 4, 5};
  demo::ServoControllers servoControllers(std::move(directionPins), std::move(i2c), channels);
  
  run(servoControllers, std::stoi(argv[1]));
  
  return 0;  
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program n [options ...]\n";
  std::cout << "    Controls the `n`-th servo controller\n";
  std::cout << "    Pressing `+` will move the servo one step up and pressing `-` will move it one step down\n";
  std::cout << "    **Note:** The servo is automatically stopped after 500ms\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void run(
    demo::ServoControllers& servoControllers,
    const std::size_t n) {
  std::vector<bool> forwards(servoControllers.numberOfControllers_, false);
  arma::Col<double> speeds(servoControllers.numberOfControllers_, arma::fill::zeros);
  speeds(n) = 1.0;
  
  std::cout << "Pressing `+` will move the servo one step up and pressing `-` will move it one step down\n";
  std::cout << "**Note:** The servo is automatically stopped after 500ms" << std::endl;
  
  bool forward;
  while(1) {
    char input;
    std::cin >> input;
    
    switch (input) {
      case '+': {
          forward = true;
          std::cout << "Moving servo " << n << " up for 500ms."<< std::endl;
        } break;
      case '-': {
          forward = false;
          std::cout << "Moving servo " << n << " down for 500ms."<< std::endl;
        } break;
      default: {
          std::cout << "You must only use '+' or '-' to control the servo." << std::endl;
          continue;
        } break;
    }
    
    forwards.at(n) = forward;
    servoControllers.run(forwards, speeds);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    servoControllers.stop();
  }
}
