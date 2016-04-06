// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

bool parseError = false;
void show_help();
void run_default();
void run_calibration();
void parse_options(
    const int argc,
    const char* argv[]);

int main (const int argc, const char* argv[]) {
  if (argc > 1 && argv[1][0] != '-') {
    if (std::string(argv[1]) == "calibrate") {
      parse_options(argc, argv);
      run_calibration();
    } else {
      ::parseError = true;
      show_help();
    }    
  } else {
    parse_options(argc, argv);
    run_default();
  }
  
  return parseError ? 1 : 0;  
}

void show_help() {
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

void run_default() {
  ::wiringPiSetupGpio();
  
  demo::Spi spi = demo::Gpio::allocateSpi();
  std::vector<unsigned int> channels = {0, 1, 2, 3, 4, 5};
  
  demo::ExtensionSensors extensionSensors(std::move(spi), channels);
  extensionSensors.setMinimalMeasurableValue(0.0); 
  extensionSensors.setMaximalMeasurableValue(1.0);
  
  while(1) {
    const std::vector<double> extensions = extensionSensors.measure();
    for (std::size_t n = 0; n < extensions.size(); ++n) {
      std::cout << "Sensor " << n << ": " << extensions.at(n) << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void run_calibration() {
  
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
