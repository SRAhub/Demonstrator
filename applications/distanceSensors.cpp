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
  
  while(1) {
    const std::vector<double> distances = distanceSensors.measure();
    for (std::size_t n = 0; n < distances.size(); ++n) {
      std::cout << "Sensor " << n << ": " << distances.at(n) << std::endl;
    }
  }
}

void run_calibration() {
  std::vector<demo::Pin> pins;
  pins.push_back(demo::Gpio::allocatePin(17));
  pins.push_back(demo::Gpio::allocatePin(27));
  pins.push_back(demo::Gpio::allocatePin(22));
  pins.push_back(demo::Gpio::allocatePin(10));
  pins.push_back(demo::Gpio::allocatePin(25));
  pins.push_back(demo::Gpio::allocatePin(11));
  demo::DistanceSensors distanceSensors(std::move(pins));
  
  while(1) {
    const std::vector<double> distances = distanceSensors.measure();
    for (std::size_t n = 0; n < distances.size(); ++n) {
      std::cout << "Sensor " << n << ": " << distances.at(n) << std::endl;
    }
  }
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
