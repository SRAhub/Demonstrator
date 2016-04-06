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
void run_sensor();
void parse_options(
    const int argc,
    const char* argv[],
    const std::size_t n);

int main (const int argc, const char* argv[]) {
  if (argc > 1 && argv[1][0] != '-') {
    if (std::string(argv[1]) == "sensor") {
      parse_options(argc, argv, 2);
      run_sensor();
    } else {
      ::parseError = true;
      show_help();
    }    
  } else {
    parse_options(argc, argv, 1);
    run_default();
  }
  
  return parseError ? 1 : 0;  
}

void show_help() {
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

void run_default() {
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
  
  while(1) {
    for (double distance = distanceIndicators.getMaximalDistance(); distance > distanceIndicators.getMinimalDistance(); distance -= 0.02) {
      std::cout << "Setting distance indication for " << distance << "m" << std::endl;
      distanceIndicators.setIndication(std::vector<double>(distanceIndicators.numberOfIndicators_, distance));
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void run_sensor() {
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
  
  while(1) {
    distanceIndicators.setIndication(distanceSensors.measure());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void parse_options(
    const int argc,
    const char* argv[],
    std::size_t n) {
  for (; n < argc; ++n) {
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
