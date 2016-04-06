#include "demonstrator_bits/sensors/attitudeSensors.hpp"

/* More information on how to use termios can be found here:
 * http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html
 */

#include <errno.h>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <math.h>

namespace demo {
  /**
   * @brief Open and set up the serial port the sensor is connected to.
   */
  AttitudeSensors::AttitudeSensors (
      Uart uart) 
    : uart_(std::move(uart)) {
    // try to open /dev/ttyAMA0; this must be explicitly enabled! (search for "/dev/ttyAMA0 raspberry pi" on the web)
    fd_ = open ("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd_ < 0) {
      throw std::runtime_error ("Couldn't open /dev/ttyAMA0\r\n\tPlease check your permissions.\r\n\tError: " + std::string (strerror (errno) ) );
    }

    // set up serial port settings
    tcgetattr (fd_, &oldTio_);
    memset (&newTio_, 0, sizeof newTio_);

    newTio_.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
    newTio_.c_iflag = IGNPAR | ICRNL;
    newTio_.c_oflag = 0;
    newTio_.c_lflag = ICANON;

    // fill in control characters
    newTio_.c_cc[VINTR]    = 0;    // Ctrl-c
    newTio_.c_cc[VQUIT]    = 0;    // Ctrl-'\'
    newTio_.c_cc[VERASE]   = 0;    // del
    newTio_.c_cc[VKILL]    = 0;    // @
    newTio_.c_cc[VEOF]     = EOF;  // Ctrl-d
    newTio_.c_cc[VTIME]    = 0;    // inter-character timer unused
    newTio_.c_cc[VMIN]     = 1;    // blocking read until 1 character arrives
    newTio_.c_cc[VSWTC]    = 0;    // \0
    newTio_.c_cc[VSTART]   = 0;    // Ctrl-q
    newTio_.c_cc[VSTOP]    = 0;    // Ctrl-s
    newTio_.c_cc[VSUSP]    = 0;    // Ctrl-z
    newTio_.c_cc[VEOL]     = 0;    // \0
    newTio_.c_cc[VREPRINT] = 0;    // Ctrl-r
    newTio_.c_cc[VDISCARD] = 0;    // Ctrl-u
    newTio_.c_cc[VWERASE]  = 0;    // Ctrl-w
    newTio_.c_cc[VLNEXT]   = 0;    // Ctrl-v
    newTio_.c_cc[VEOL2]    = 0; //'\n'; // \0

    // set i/o speed/baudrate
    cfsetispeed (&newTio_, B57600);
    cfsetospeed (&newTio_, B57600);
    tcflush (fd_, TCIFLUSH);
    tcsetattr (fd_, TCSANOW, &newTio_);
  }



  /**
   * @brief Close the serial port.
   */
  AttitudeSensors::~AttitudeSensors () {
    // reset port to previous state
    tcsetattr (fd_, TCSANOW, &oldTio_);
    if (fd_ != -1) {
      close (fd_);
    }
  }



  /**
   * @brief   Returns the rotation of the sensor (roll/pitch/yaw).
   * @return  Rotation values (roll/pitch/yaw).
   */
  std::vector<double> AttitudeSensors::measureImplementation () const {
    std::vector<double> result;
    int rec = 0;
    char buf[64];

    // read input from serial port
    // TODO: move "2-chars-minimum-check" out of this function (maybe blocks the program if there is no input on serial port)
    while (rec <= 1) {
      rec = read (fd_, buf, 63);
    }

    // parse results
    buf[rec] = 0;
    std::string text = buf;
    text = text.substr (text.find_first_of ("=") + 1);
    result.push_back(stod (text.substr (0, text.find_first_of (",") ) ) * M_PI / 180.0);
    result.push_back(stod (text.substr (text.find_first_of (",") + 1, text.find_last_of (",") ) ) * M_PI / 180.0);
    result.push_back(stod (text.substr (text.find_last_of (",") + 1) ) * M_PI / 180.0);

    return result;
  }

  /**
   * @brief Set the current position to (0, 0, 0).
   */
  void AttitudeSensors::reset () {
    write (fd_, "#r", 2);
  }
}