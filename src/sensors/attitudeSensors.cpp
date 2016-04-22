#include "demonstrator_bits/sensors/attitudeSensors.hpp"

// C++ standard library
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>

// Unix library
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/* More information on how to use termios can be found here:
 * http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html
 */

namespace demo {
  /**
   * @brief Open and set up the serial port the sensor is connected to.
   */
  AttitudeSensors::AttitudeSensors(
      Uart&& uart,
      const double minimalAttitude,
      const double maximalAttitude)
      : Sensors(3, minimalAttitude, maximalAttitude),
        uart_(std::move(uart)) {
          
  }

  AttitudeSensors::AttitudeSensors(
      AttitudeSensors&& attitudeSensors)
      : AttitudeSensors(std::move(attitudeSensors.uart_), attitudeSensors.minimalMeasurableValue_, attitudeSensors.maximalMeasurableValue_) {
    setMeasurementCorrections(attitudeSensors.measurementCorrections_);
    setNumberOfSamplesPerMeasurment(attitudeSensors.numberOfSamplesPerMeasuement_);
  }

  AttitudeSensors& AttitudeSensors::operator=(
      AttitudeSensors&& attitudeSensors) {
    uart_ = std::move(attitudeSensors.uart_);

    Sensors::operator=(std::move(attitudeSensors));
    return *this;
  }

  AttitudeSensors::~AttitudeSensors() {
    if (continuousMeasurementThread_.joinable()) {
      killContinuousMeasurementThread_ = true;
      continuousMeasurementThread_.join();
    }
    
    // reset port to previous state
    if (fileDescriptor_ != -1) {
      ::tcsetattr(fileDescriptor_, TCSANOW, &oldSerial_);
      ::close(fileDescriptor_);
    }
  }

  arma::Row<double> AttitudeSensors::measureImplementation() {
    return attitudes_;
  }
  
  void AttitudeSensors::runAsynchronous() {
    // try to open /dev/ttyAMA0; this must be explicitly enabled! (search for "/dev/ttyAMA0 raspberry pi" on the web)
    fileDescriptor_ = ::open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fileDescriptor_ < 0) {
      throw std::runtime_error("AttitudeSensors: Could not access /dev/ttyAMA0");
    }

    // set up serial port settings
    ::tcgetattr(fileDescriptor_, &oldSerial_);
    std::memset(&newSerial_, 0, sizeof(newSerial_));

    newSerial_.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
    newSerial_.c_iflag = IGNPAR | ICRNL;
    newSerial_.c_oflag = 0;
    newSerial_.c_lflag = ICANON;

    // fill in control characters
    newSerial_.c_cc[VINTR] = 0; // Ctrl-c
    newSerial_.c_cc[VQUIT] = 0; // Ctrl-'\'
    newSerial_.c_cc[VERASE] = 0; // del
    newSerial_.c_cc[VKILL] = 0; // @
    newSerial_.c_cc[VEOF] = EOF; // Ctrl-d
    newSerial_.c_cc[VTIME] = 0; // inter-character timer unused
    newSerial_.c_cc[VMIN] = 1; // blocking read until 1 character arrives
    newSerial_.c_cc[VSWTC] = 0; // \0
    newSerial_.c_cc[VSTART] = 0; // Ctrl-q
    newSerial_.c_cc[VSTOP] = 0; // Ctrl-s
    newSerial_.c_cc[VSUSP] = 0; // Ctrl-z
    newSerial_.c_cc[VEOL] = 0; // \0
    newSerial_.c_cc[VREPRINT] = 0; // Ctrl-r
    newSerial_.c_cc[VDISCARD] = 0; // Ctrl-u
    newSerial_.c_cc[VWERASE] = 0; // Ctrl-w
    newSerial_.c_cc[VLNEXT] = 0; // Ctrl-v
    newSerial_.c_cc[VEOL2] = 0; //'\n'; // \0

    // set i/o speed/baudrate
    ::cfsetispeed(&newSerial_, B57600);
    ::cfsetospeed(&newSerial_, B57600);
    ::tcflush(fileDescriptor_, TCIFLUSH);
    ::tcsetattr(fileDescriptor_, TCSANOW, &newSerial_);

    killContinuousMeasurementThread_ = false;
    continuousMeasurementThread_ = std::thread(&AttitudeSensors::asynchronousMeasurement, this);
  }

  void AttitudeSensors::asynchronousMeasurement() {
    while (!killContinuousMeasurementThread_) {
      char buffer[64];

      int numberOfReceivedChars = 0;
      while (numberOfReceivedChars <= 1) {
        numberOfReceivedChars = ::read(fileDescriptor_, buffer, 63);
      }
      buffer[numberOfReceivedChars] = '\0';

      std::string text = buffer;
      text = text.substr(text.find_first_of("=") + 1);
      attitudes_(0) = std::stod(text.substr(0, text.find_first_of(",")));
      attitudes_(1) = std::stod(text.substr(text.find_first_of(",") + 1, text.find_last_of(",")));
      attitudes_(2) = std::stod(text.substr(text.find_last_of(",") + 1));
      attitudes_ *= arma::datum::pi / 180.0;
      
    }
  }

  /**
   * @brief Set the current position to (0, 0, 0).
   */
  void AttitudeSensors::reset() {
    if (::write(fileDescriptor_, "#r", 2) == -1) {
      throw std::runtime_error("AttitudeSensors: Could not reset sensor");
    }
  }
}
