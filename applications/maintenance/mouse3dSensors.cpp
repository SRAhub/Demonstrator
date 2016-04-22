// C++ standard library
#include <chrono>
#include <iomanip>
#include <thread>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runDefault(
    demo::Mouse3dSensors&);

int main (const int argc, const char* argv[]) {
  if (hasOption(argc, argv, "-h") || hasOption(argc, argv, "--help")) {
    showHelp();
    // Terminates the program after the help is shown.
    return 0;
  }

  if (hasOption(argc, argv, "--verbose")) {
    ::demo::isVerbose = true;
  }

  demo::Mouse3dSensors mouse3dSensors(-1, 1);

  runDefault(mouse3dSensors);

  return 0;
}

void showHelp() {
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

void runDefault(
    demo::Mouse3dSensors& mouse3dSensors) {
  mouse3dSensors.runAsynchronous();

  while(1) {
    std::cout << "+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+\n"
              << "|    X_pos    |    Y_pos    |    Z_pos    |    X_rot    |    Y_rot    |    Z_rot    |    Btn_1    |    Btn_2    |\n"
              << "+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+" << std::endl;
    for (unsigned int n = 0; n < 10; ++n) {
      const arma::Row<double>& poses = mouse3dSensors.measure();
      std::cout << "|";
      for (std::size_t k = 0; k < mouse3dSensors.numberOfSensors_; ++k) {
         std::cout << " " << std::setw(11) << poses(k) << " |";
      }
      std::cout << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }
}
