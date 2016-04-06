// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

bool parseError = false;
void show_help();
void run(
    const std::size_t n);
void parse_options(
    const int argc,
    const char* argv[]);

int main (const int argc, const char* argv[]) {
  std::size_t n = std::stoi(argv[1]);
  
  if (argc < 2 || n > 5) {
    ::parseError = true;
    show_help();
  }
  
  parse_options(argc, argv);
  run(n);
  
  return ::parseError ? 1 : 0;  
}

void show_help() {
  std::cout << "Usage:\n";
  std::cout << "  program n [options ...]\n";
  std::cout << "    Uses the `n`-th servo controller.\n";
  std::cout << "    Pressing `+` will move the servo one step up and pressing `-` will move it one step down.\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void run(
    const std::size_t n) {
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
  
  std::vector<bool> forwards = {false, false, false, false, false, false};
  std::vector<double> speeds = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  speeds.at(n) = 1.0;
  
  bool direction;
  while(1) {
    char input;
    std::cin >> input;
    if (input == '+') {
      direction = true;
    } else if (input == '+') {
      direction = false;
    } else {
      throw std::domain_error("You must only use '+' or '-' to control the servo.");
    }
    
    forwards.at(n) = direction;
    servoControllers.run(forwards, speeds);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    servoControllers.stop();
  }
}

void parse_options(
    const int argc,
    const char* argv[]) {
  for (std::size_t n = 2; n < argc; ++n) {
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
