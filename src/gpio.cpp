#include "demonstrator_bits/gpio.hpp"
#include "demonstrator_bits/config.hpp" // IWYU pragma: keep

// C++ standard library
#include <mutex>
#include <string>

// Demonstrator
#include "demonstrator_bits/assert.hpp"

namespace demo {
  // Sets all elements in `ownedPins_` to false.
  decltype(Gpio::ownedPins_) Gpio::ownedPins_({});
#if !defined(USE_WIRINGPI_DUMMY)
  decltype(Gpio::i2cFileDescriptor_) Gpio::i2cFileDescriptor_(::wiringPiI2CSetup(0x40));
#else
  decltype(Gpio::i2cFileDescriptor_) Gpio::i2cFileDescriptor_(0);
#endif
  decltype(Gpio::mutex_) Gpio::mutex_;

  Pin Gpio::allocatePin(
      const unsigned int pinNumber) {
    std::lock_guard<std::mutex> lock(mutex_);

    // GPIO pin numbering ranges from 2 to 27
    verify(1 < pinNumber && pinNumber < 28, "Invalid physical pin number: " + std::to_string(pinNumber));
    verify(!ownedPins_.at(pinNumber - 2), "Pin " + std::to_string(pinNumber) + " already allocated");

    ownedPins_.at(pinNumber - 2) = true;
    return Pin(pinNumber);
  }

  Spi Gpio::allocateSpi() {
    std::lock_guard<std::mutex> lock(mutex_);

    verify(!ownedPins_.at(2), "Pin 2 already allocated");
    verify(!ownedPins_.at(3), "Pin 3 already allocated");

    ownedPins_.at(2) = true;
    ownedPins_.at(3) = true;

    return Spi();
  }

  I2c Gpio::allocateI2c() {
    std::lock_guard<std::mutex> lock(mutex_);

    verify(!ownedPins_.at(2), "Pin 2 already allocated");
    verify(!ownedPins_.at(3), "Pin 3 already allocated");

    ownedPins_.at(2) = true;
    ownedPins_.at(3) = true;

    return I2c(i2cFileDescriptor_);
  }

  Uart Gpio::allocateUart() {
    std::lock_guard<std::mutex> lock(mutex_);

    verify(!ownedPins_.at(2), "Pin 2 already allocated");
    verify(!ownedPins_.at(3), "Pin 3 already allocated");

    ownedPins_.at(2) = true;
    ownedPins_.at(3) = true;

    return Uart();
  }

  void Gpio::deallocate(
      Pin& pin) {
    if (pin.ownsPin_) {
      ownedPins_.at(pin.pinNumber_ - 2) = false;
      pin.ownsPin_ = false;
    }
  }

  void Gpio::deallocate(
      Spi& spi) {
    if (spi.ownsSpi_) {
      ownedPins_.at(2) = false;
      ownedPins_.at(3) = false;
      spi.ownsSpi_ = false;
    }
  }

  void Gpio::deallocate(
      I2c& i2c) {
    if (i2c.ownsI2c_) {
      ownedPins_.at(2) = false;
      ownedPins_.at(3) = false;
      i2c.ownsI2c_ = false;
    }
  }

  void Gpio::deallocate(
      Uart& uart) {
    if (uart.ownsUart_) {
      ownedPins_.at(2) = false;
      ownedPins_.at(3) = false;
      uart.ownsUart_ = false;
    }
  }
}
