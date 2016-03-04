#pragma once

namespace demo {
  class Uart {
    friend class Gpio;

   public:
    Uart& operator=(Uart&) = delete;
    Uart(Uart&) = delete;

    Uart(Uart&&);

    Uart& operator=(Uart&&);

    virtual ~Uart();

   private:
    Uart();

    bool ownsUart_;
  };
}
