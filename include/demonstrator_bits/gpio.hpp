#pragma once

// C++ standard library
#include <array>
#include <mutex>

// Demonstrator
#include "demonstrator_bits/pin.hpp"
#include "demonstrator_bits/spi.hpp"
#include "demonstrator_bits/i2c.hpp"
#include "demonstrator_bits/uart.hpp"

namespace demo {
  class Gpio {
   public:
    /**
     * Number of GPIO pins on A+, B+ and Pi 2 models.
     */
    static const int GPIO_PIN_COUNT = 26;

    Gpio() = delete;
    Gpio(Gpio&) = delete;
    Gpio operator=(Gpio&) = delete;

    /**
     * Ask for ownership of the pin with the specified number.
     *
     * Throws `domain_error` if the pin is already allocated, and `out_of_range` if a pin with that number doesn't exist on raspberry pi.
     */
    static Pin allocatePin(
        const unsigned int pinNumber);

    static Spi allocateSpi();

    static I2c allocateI2c();

    static Uart allocateUart();

    /**
     * Give ownership over `pin` back to the array.
     *
     * Pins automatically deallocate themselves if they are deleted, so you only need to call this method explicitly if you want to deallocate the pin before the end of that objects lifecycle.
     */
    static void deallocate(
        Pin& pin);

    static void deallocate(
        Spi& spi);

    static void deallocate(
        I2c& i2c);

    static void deallocate(
        Uart& uart);

   protected:
    static std::array<bool, GPIO_PIN_COUNT> ownedPins_;

    static int i2cFileDescriptor_;

    static std::mutex mutex_;
  };
}
