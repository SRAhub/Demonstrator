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
void runDefault(
    demo::StewartPlatform& stewartPlatform);
void runEndEffectorPose(
    demo::StewartPlatform& stewartPlatform,
    const arma::Col<double>::fixed<6>& endEffectorPose);
void runEvasion(
    demo::StewartPlatform& stewartPlatform,
    demo::Network& network);

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
  servoControllers.setMinimalSpeed(0.75);
  servoControllers.setMaximalSpeed(1.0);
  
  demo::Spi spi = demo::Gpio::allocateSpi();
  std::vector<unsigned int> spiChannels = {0, 1, 2, 3, 4, 5};
  demo::ExtensionSensors extensionSensors(std::move(spi), spiChannels);
  extensionSensors.setMinimalMeasurableValue(0.1);
  extensionSensors.setMaximalMeasurableValue(1.0);
  
  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors));
  linearActuators.setMinimalExtension(0.1);
  linearActuators.setMaximalExtension(0.8);
  linearActuators.setMaximalExtensionDeviation(0.05);
  
  demo::Uart uart = demo::Gpio::allocateUart();
  demo::AttitudeSensors attitudeSensors(std::move(uart));
  attitudeSensors.setMinimalMeasurableValue(-arma::datum::pi); 
  attitudeSensors.setMaximalMeasurableValue(arma::datum::pi);
  
  arma::Mat<double>::fixed<3, 6> baseJointsPosition;
  baseJointsPosition.load("baseJointsPosition.mat");
  arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
  endEffectorJointsRelativePosition.load("endEffectorJointsRelativePosition.mat");
  arma::Row<double>::fixed<6> actuatorsMinimalLength;
  actuatorsMinimalLength.load("actuatorsMinimalLength.mat");
  arma::Row<double>::fixed<6> actuatorsMaximalLength;
  actuatorsMaximalLength.load("actuatorsMaximalLength.mat");
  
  demo::StewartPlatform stewartPlatform(std::move(linearActuators), std::move(attitudeSensors), baseJointsPosition, endEffectorJointsRelativePosition, actuatorsMinimalLength, actuatorsMaximalLength);
  
  demo::Network network(9001);
  
  if (hasOption(argc, argv, "evasion")) {
    runEvasion(stewartPlatform, network);
  } else if (argc > 6 & argv[1][0] != '-' & argv[2][0] != '-' & argv[3][0] != '-' & argv[4][0] != '-' & argv[5][0] != '-' & argv[6][0] != '-') {
    runEndEffectorPose(stewartPlatform, {std::stod(argv[1]), std::stod(argv[2]), std::stod(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6])});
  } else {
    runDefault(stewartPlatform);
  }
  
  return 0;  
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Moves one actuator after another up by 15% and then down again\n";
  std::cout << "\n";
  std::cout << "  program x y z roll pitch yaw [options ...]\n";
  std::cout << "    Translates the Stewart platform to (`x`, `y`, `z`) and using (`roll`, `pitch`, `yaw`) as rotation\n";
  std::cout << "    The translations must be given in [m] and the rotations in [radian]\n";
  std::cout << "    The (0, 0, 0) position is located in the middle of the base\n";
  std::cout << "\n";
  std::cout << "  program extension [options ...]\n";
  std::cout << "    Moves all actuators to `extension`\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runDefault(
    demo::StewartPlatform& stewartPlatform) {
  arma::Col<double>::fixed<6> endEffectorPose = stewartPlatform.getEndEffectorPose();
  
  while(1) {
    std::cout << "Moving the Stewart platform along the x-axis by 3cm." << std::endl;
    endEffectorPose(0) -= 0.03;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(0) += 0.03;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(0) = 0.0;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Moving the Stewart platform along the y-axis by 3cm." << std::endl;
    endEffectorPose(1) -= 0.03;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(1) += 0.03;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(1) = 0.0;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Moving the Stewart platform along the z-axis by 3cm." << std::endl;
    endEffectorPose(2) -= 0.03;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(2) += 0.03;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(2) = 0.0;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Tilting the Stewart platform along the roll-axis by 15 degree." << std::endl;
    endEffectorPose(3) += 0.26;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(3) -= 0.26;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(3) = 0.0;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Tilting the Stewart platform along the pitch-axis by 15 degree." << std::endl;
    endEffectorPose(4) += 0.26;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(4) -= 0.26;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(4) = 0.0;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Rotating the Stewart platform along the yaw-axis by 15 degree." << std::endl;
    endEffectorPose(5) += 0.26;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(5) -= 0.26;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    endEffectorPose(5) = 0.0;
    stewartPlatform.setEndEffectorPose(endEffectorPose);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void runEndEffectorPose(
    demo::StewartPlatform& stewartPlatform,
    const arma::Col<double>::fixed<6>& endEffectorPose) {
  stewartPlatform.setEndEffectorPose(endEffectorPose);
}

void runEvasion(
    demo::StewartPlatform& stewartPlatform,
    demo::Network& network) {
      
}
