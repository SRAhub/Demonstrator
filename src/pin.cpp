#include "demonstrator_bits/pin.hpp"

// C++ standard library
#include <ratio>
#include <string>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include "demonstrator_bits/assert.hpp"
#include "demonstrator_bits/gpio.hpp"

namespace demo {
  Pin::Pin(
      const unsigned int pinNumber)
      : pinNumber_(pinNumber),
        ownsPin_(true) {}

  Pin::Pin(Pin&& other)
      : pinNumber_(other.pinNumber_),
        ownsPin_(other.ownsPin_) {
    other.ownsPin_ = false;
  }

  Pin& Pin::operator=(Pin&& other) {
    if (ownsPin_) {
      Gpio::deallocate(*this);
    }

    pinNumber_ = other.pinNumber_;
    ownsPin_ = other.ownsPin_;

    other.ownsPin_ = false;
    return *this;
  }

  void Pin::set(
      const Pin::Digital value) {
    verify(ownsPin_, "The ownership of pin " + std::to_string(pinNumber_) + " has been transferred");

    ::pinMode(static_cast<int>(pinNumber_), OUTPUT);
    ::digitalWrite(static_cast<int>(pinNumber_), static_cast<int>(value));
  }

  void Pin::set(
      const unsigned int value) {
    set(value ? Digital::High : Digital::Low);
  }

  Pin::Digital Pin::get() {
    verify(ownsPin_, "The ownership of pin " + std::to_string(pinNumber_) + " has been transferred");

    ::pinMode(static_cast<int>(pinNumber_), INPUT);
    return (::digitalRead(static_cast<int>(pinNumber_)) == 0 ? Digital::Low : Digital::High);
  }

  std::chrono::microseconds Pin::waitForSignalEdge(
      const std::chrono::microseconds timeout) {
    verify(ownsPin_, "The ownership of pin " + std::to_string(pinNumber_) + " has been transferred");

    Digital currentSignal = get();

    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    while (get() == currentSignal) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(500));
      end = std::chrono::steady_clock::now();

      if (end - start >= timeout) {
        return timeout;
      }
    }

    return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  }

  Pin::~Pin() {
    if (ownsPin_) {
      Gpio::deallocate(*this);
    }
  }
}
