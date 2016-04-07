// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

bool parseError = false;
void show_help();
void run();
void parse_options(
    const int argc,
    const char* argv[]);

int main (const int argc, const char* argv[]) {
  if (argc > 1 && argv[1][0] != '-') {
    ::parseError = true;
    show_help();    
  } else {
    parse_options(argc, argv);
    run();
  }
  
  return parseError ? 1 : 0;  
}

void show_help() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Prints measurements for each sensor until the program is terminated\n";
  std::cout << "\n";
  std::cout << "  program single [options ...]\n";
  std::cout << "    Starts the sensor calibration\n";
  std::cout << "\n";
  std::cout << "  program all [options ...]\n";
  std::cout << "    Starts the sensor calibration\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void run() {
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
  
  demo::Spi spi = demo::Gpio::allocateSpi();
  std::vector<unsigned int> spiChannels = {0, 1, 2, 3, 4, 5};
  demo::ExtensionSensors extensionSensors(std::move(spi), spiChannels);
  extensionSensors.setMinimalMeasurableValue(0.1);
  extensionSensors.setMaximalMeasurableValue(1.0);
  
  std::vector<double> extensions = extensionSensors.measure();
  const std::vector<double> maximalSpeeds = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  
  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors));
  linearActuators.setMinimalSpeed(0.5);
  linearActuators.setMaximalSpeed(1.0);
  linearActuators.setMaximalExtensionDeviation(0.05);
  
  demo::StewartPlatform stewartPlatform(std::move(linearActuators));
  stewartPlatform.setEndEffectorPose({0.0, 0.0, 0.03, 0.0, 0.0, 0.0});
}

void parse_options(
    const int argc,
    const char* argv[]) {
  for (std::size_t n = 1; n < argc; ++n) {
    std::string option = argv[n];
    
    if ((option == "-h") || (option == "--help")) {
      show_help();
    } else if (option == "--verbose") {
      ::demo::isVerbose = true;
    } else {
      ::parseError = true;
      show_help();
    }
  }
}
