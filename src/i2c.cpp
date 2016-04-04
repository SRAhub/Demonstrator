#include "demonstrator_bits/i2c.hpp"

// WiringPi
#include <wiringPiI2C.h>

// Demonstrator
#include "demonstrator_bits/gpio.hpp"

namespace demo {
  I2c::I2c()
      : fileDescriptor_(::wiringPiI2CSetup(0x40)),
        ownsI2c_(true) {}

  I2c::I2c(I2c&& other)
      : fileDescriptor_(other.fileDescriptor_),
        ownsI2c_(other.ownsI2c_) {
    other.ownsI2c_ = false;
  }

  I2c& I2c::operator=(I2c&& other) {
    if (ownsI2c_) {
      Gpio::deallocate(*this);
    }

    fileDescriptor_ = other.fileDescriptor_,
    ownsI2c_ = other.ownsI2c_;

    other.ownsI2c_ = false;
    return *this;
  }

  void I2c::set(
      const unsigned int registerNumber,
      const unsigned int value) {
    if (ownsI2c_) {
      throw std::runtime_error("I2C must be owned to be accessed.");
    }

    ::wiringPiI2CWriteReg8(fileDescriptor_, static_cast<int>(registerNumber), static_cast<int>(value));
  }

  unsigned int I2c::get(
      const unsigned int registerNumber) {
    if (ownsI2c_) {
      throw std::runtime_error("I2C must be owned to be accessed.");
    }

    return static_cast<unsigned int>(::wiringPiI2CReadReg8(fileDescriptor_, static_cast<int>(registerNumber)));
  }

  I2c::~I2c() {
    if (ownsI2c_) {
      Gpio::deallocate(*this);
    }
  }
}
