#pragma once
#include "demonstrator_bits/config.hpp"

// C++ standard library
#include <chrono>

/**
 * Instances of this class represent a single GPIO pin on the Raspberry Pi.
 *
 * The concept of this class is to protect you against bugs that arise from accidental pin use in multiple modules of your software: The wiringPi library represents pins as simple integers, which makes it hard to track where a in a program a certain pin is used. If you read from or write to the same pin in different modules by mistake, it can't be detected by the compiler or at runtime.
 *
 * To solve this you can use this class: It represents each pin as an object that has to be obtained from `::demo::Gpio`, which will throw an exception if you try to allocate a pin twice. In return, the pins are not constructible or copyable. If a pin object is destroyed, it assigns itself back to the GPIO array automatically and is available for allocation again.
 */
namespace demo {
  class Pin {
    /**
     * This class may only be instantiated by `::demo::Gpio`, as its only purpose is to enforce single pin ownership.
     */
    friend class Gpio;

   public:
    /**
     * Encodes the digital signals you can put on a pin.
     */
    enum class Digital : unsigned int {
      Low = 0, // LOW,
      High = 1 // HIGH
    };

    Pin& operator=(Pin&) = delete;
    Pin(Pin&) = delete;

    /**
     * Take over ownership from `other`.
     */
    Pin(Pin&&);

    /**
     * Take over ownership from `other`.
     */
    Pin& operator=(Pin&&);

    /**
     * Switch this pin to `output` mode and set it to the assigned value.
     */
    void set(
        const Digital value);

    /**
     * Alias of `send(Digital)`. Maps 0 to Digital::low and every other value to Digital::high.
     */
    void set(
        const unsigned int value);

    /**
     * Switch this pin to `input` mode and read the current value.
     */
    Digital get();

    /**
     * Switch this pin to `input` mode, then wait for the next signal edge and return the time difference. Abort after `timeout` microseconds.
     */
    std::chrono::microseconds waitForSignalEdge(
        const std::chrono::microseconds timeout);

    /**
     * If this object currently owns its pin, pass that ownership back to the gpio array.
     */
    virtual ~Pin();

   protected:
    /**
     * The constructor is private, as only Gpio (which is a friend) may instantiate this class.
     */
    Pin(
        const unsigned int pinNumber);

    /**
     * Throw a `std::domain_error` if this object doesn't own its pin.
     */
    void assertOwnership();

    /**
     * The pin number of of this pin, using BCM GPIO numbering.
     */
    unsigned int pinNumber_;

    /**
     * Indicates that this object still owns its pin (ownership has not been transferred).
     */
    bool ownsPin_;
  };
}
