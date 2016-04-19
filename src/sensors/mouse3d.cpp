#include "demonstrator_bits/sensors/mouse3d.hpp"

// C++ standard library
#include <cerrno>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>

// Unix library
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

namespace demo {
  Mouse3d::Mouse3d()
      : Sensors(8) {
        
  }

  Mouse3d::Mouse3d(
      Mouse3d&& mouse3d)
      : Sensors(8) {
    fileDescriptor_ = mouse3d.fileDescriptor_;
    mouse3d.fileDescriptor_ = -1;
  }

  Mouse3d& Mouse3d::operator=(
      Mouse3d&& mouse3d) {
    fileDescriptor_ = mouse3d.fileDescriptor_;
    mouse3d.fileDescriptor_ = -1;

    return *this;
  }

  Mouse3d::~Mouse3d() {
    killContinuousMeasurementThread_ = true;
    continuousMeasurementThread_.join();

    if (fileDescriptor_ != -1) {
      ::close(fileDescriptor_);
    }
  }

  arma::Row<double> Mouse3d::measureImplementation() {
    return displacements_;
  }
  
  void Mouse3d::runAsynchronous() {
    std::size_t maximalNumberOfEventDeviceIds = 32;
    for (std::size_t n = 0; n < maximalNumberOfEventDeviceIds; n++) {
      fileDescriptor_ = ::open(("/dev/input/event" + std::to_string(n)).c_str(), O_RDWR | O_NONBLOCK);
      if (fileDescriptor_ > 0) {
        struct ::input_id inputId;
        ::ioctl(fileDescriptor_, EVIOCGID, &inputId);

        if (inputId.vendor == 0x046d && (inputId.product == 0xc626 || inputId.product == 0xc621 || inputId.product == 0xc623 || inputId.product == 0xc603)) {
          uint8_t eventTypeBitmask[EV_MAX / 8 + 1];
          ::ioctl(fileDescriptor_, EVIOCGBIT(0, sizeof(eventTypeBitmask)), eventTypeBitmask);

          killContinuousMeasurementThread_ = false;
          continuousMeasurementThread_ = std::thread(&Mouse3d::asynchronousMeasurement, this);

          return;
        }
      }
    }

    throw std::runtime_error("Mouse3d: Could not connect to the 3d mouse.");
  }

  void Mouse3d::asynchronousMeasurement() {
    while (!killContinuousMeasurementThread_) {
      struct ::input_event inputEvent;

      int datasize = ::read(fileDescriptor_, &inputEvent, sizeof(inputEvent));

      if (datasize < 0) {
        throw std::runtime_error("Mouse3d.continuousMeasurement: " + static_cast<std::string>(std::strerror(errno)));
      }

      if (datasize >= sizeof(inputEvent)) {
        switch (inputEvent.type) {
          case EV_KEY: {
            displacements_(6 + inputEvent.code % 2) = inputEvent.value;
          } break;
          case EV_REL:
          case EV_ABS: {
            displacements_(inputEvent.code) = inputEvent.value;
          } break;
          default:
            break;
        }
      }
    }
  }
}