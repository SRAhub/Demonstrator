#include "demonstrator_bits/uart.hpp"

// Demonstrator
#include "demonstrator_bits/gpio.hpp"

namespace demo {
  Uart::Uart()
      : ownsUart_(true) {}

  Uart::Uart(Uart&& other)
      : ownsUart_(other.ownsUart_) {
    other.ownsUart_ = false;
  }

  Uart& Uart::operator=(Uart&& other) {
    if (ownsUart_) {
      Gpio::deallocate(*this);
    }

    ownsUart_ = other.ownsUart_;

    other.ownsUart_ = false;
    return *this;
  }

  Uart::~Uart() {
    if (ownsUart_) {
      Gpio::deallocate(*this);
    }
  }
}
