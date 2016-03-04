#pragma once

namespace demo {
  class I2c {
    friend class Gpio;

   public:
    I2c& operator=(I2c&) = delete;
    I2c(I2c&) = delete;

    I2c(I2c&&);

    I2c& operator=(I2c&&);

    void set(
        const unsigned int register,
        const unsigned int value);

    unsigned int get(
        const unsigned int register);

    virtual ~I2c();

   protected:
    I2c(
        const int fileDescriptor);

    int fileDescriptor_;

    bool ownsI2c_;
  };
}
