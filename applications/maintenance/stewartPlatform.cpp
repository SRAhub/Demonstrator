// C++ standard library
#include <chrono>
#include <iomanip>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runDefault(
    demo::StewartPlatform& stewartPlatform);
void runSensor(
    demo::StewartPlatform& stewartPlatform);
void runEndEffectorPose(
    demo::StewartPlatform& stewartPlatform,
    const arma::Col<double>::fixed<6>& endEffectorPose);

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
  linearActuators.setAcceptableExtensionDeviation(0.005);
  
  demo::AttitudeSensors attitudeSensors(demo::Gpio::allocateUart(), -arma::datum::pi, arma::datum::pi);
  
  arma::Mat<double>::fixed<3, 6> baseJointsPosition;
  baseJointsPosition.load("baseJointsPosition.config");
  arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition;
  endEffectorJointsRelativePosition.load("endEffectorJointsRelativePosition.config");
  
  demo::StewartPlatform stewartPlatform(std::move(linearActuators), std::move(attitudeSensors), baseJointsPosition, endEffectorJointsRelativePosition, {-0.02, -0.02, 0.21, -0.2, -0.2, -0.6}, {0.02, 0.02, 0.27, 0.2, 0.2, 0.6});
  
  if (hasOption(argc, argv, "sensor")) {
    runSensor(stewartPlatform);
  } if (argc > 6 && isNumber(argv[1]) && isNumber(argv[2]) && isNumber(argv[3]) && isNumber(argv[4]) && isNumber(argv[5]) && isNumber(argv[6])) {
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
  std::cout << "  program sensor [options ...]\n";
  std::cout << "    Moves all actuators to extension\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runDefault(
    demo::StewartPlatform& stewartPlatform) {
  arma::Col<double>::fixed<6> endEffectorPose = {0, 0, 0.25, 0, 0, 0};

  std::cout << "Moving the Stewart platform to (0, 0, 0.25, 0, 0, 0)." << std::endl;
  stewartPlatform.setEndEffectorPose(endEffectorPose);
  stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
  
  while(1) {
    for (unsigned int n = 0; n < 2; ++n) {
      std::cout << "Moving the Stewart platform along axis " <<  (n + 1) << " down by 3cm." << std::endl;
      endEffectorPose(n) -= 0.02;
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::cout << "Moving the Stewart platform along axis " <<  (n + 1) << " up by 3cm." << std::endl;
      endEffectorPose(n) += 0.04;
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::cout << "Moving the Stewart platform along axis " <<  (n + 1) << " to 0cm." << std::endl;
      endEffectorPose(n) = 0.0;
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    for (unsigned int n = 3; n < 5; ++n) {
      std::cout << "Moving the Stewart platform along axis " <<  (n + 1) << " up by 15 degree." << std::endl;
      endEffectorPose(n) -= 0.14;
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::cout << "Moving the Stewart platform along axis " <<  (n + 1) << " down by 30 degree." << std::endl;
      endEffectorPose(n) += 0.28;
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::cout << "Moving the Stewart platform along axis " <<  (n + 1) << " to 0 degree." << std::endl;
      endEffectorPose(n) = 0.0;
      stewartPlatform.setEndEffectorPose(endEffectorPose);
      stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void runSensor(
    demo::StewartPlatform& stewartPlatform) {
  while(1) {
    std::cout << "+-----------------+-----------------+-----------------+-----------------+-----------------+-----------------+\n"
              << "|   X-axis [m]    |   Y-axis  [m]   |   Z-axis [m]    | Roll [radians]  | Pitch [radians] |  Yaw [radians]  |\n"
              << "+-----------------+-----------------+-----------------+-----------------+-----------------+-----------------+" << std::endl;
    for (unsigned int n = 0; n < 10; ++n) {
      const arma::Col<double>::fixed<6>& endEffectorPose = stewartPlatform.getEndEffectorPose();
      std::cout << "|";
      for (std::size_t k = 0; k < endEffectorPose.n_elem; ++k) {
         std::cout << " " << std::setw(15) << endEffectorPose(k) << " |";
      }
      std::cout << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void runEndEffectorPose(
    demo::StewartPlatform& stewartPlatform,
    const arma::Col<double>::fixed<6>& endEffectorPose) {
  stewartPlatform.setEndEffectorPose(endEffectorPose);
  stewartPlatform.waitTillEndEffectorPoseIsReached(std::chrono::seconds(10));
}
