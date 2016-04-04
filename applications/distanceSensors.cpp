// Demonstrator
#include <demonstrator>

std::vector<demo::Pin> pins = {
  demo::Gpio::allocatePin(1),
  demo::Gpio::allocatePin(2),
  demo::Gpio::allocatePin(3),
  demo::Gpio::allocatePin(4),
  demo::Gpio::allocatePin(5),
  demo::Gpio::allocatePin(6)
};

void show_help();
void run_default();
void run_calibration();
void parse_options();

int main (const int argc, char* argv[]) {
  if (argc > 1 && argv[1][0] != "-") {
    if (std::string(argv[1]) == "calibrate") {
      parse_options();
      run_calibration();
    } else {
      show_help();
      return 1;
    }    
  } else {
    parse_options();
    run_default();
  }
  
  return 0;  
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
  std::flush;
}

void run_default() {
  demo::DistanceSensors distanceSensors(pins);
  while(1) {
    const std::vector<double> distances = distanceSensors.measure();
    for (std::size_t n = 0; n < distances.size(); ++n) {
      std::cout << "Sensor " << n << ": " << distances.at(n) << std::endl;
    }
  }
}

void run_calibration() {
  std::cout << "run_calibration()" << std::endl;
}

parse_options() {
  for (int n = 1; n < argc; ++n) {
    std::string option = argv[i];
    
    if ((option == "-h") || (option == "--help")) {
      show_usage(argv[0]);
      return 0;
    } else if (option == "--verbose") {
      ::demo::isVerbose = true;
    } else {
      show_help();
      return 1;
    }
  }
}
