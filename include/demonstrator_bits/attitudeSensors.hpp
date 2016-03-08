#pragma once

// Armadillo
#include <armadillo>

namespace demo {

  /**
   * Represents a single [AHRS sensor][1].
   *
   * [1]: TODO find product page, insert link!
   */
  class AttitudeSensor {
   public:
    AttitudeSensor();

    /**
     * Read and return the current attitute from the sensor. The order of values is roll / pitch / yaw, and each value is in the range [0, 360[.
     */
    arma::Col<double>::fixed<3> getAttitude() const;

    /**
     * Designate the current attitute to be zero, and all further measurements be relative to it.
     */
    void reset();

    void setSensorAdjustment(
        const arma::Col<double>::fixed<3>& sensorAdjustment);
    arma::Col<double>::fixed<3> getSensorAdjustment() const;

   protected:

    /**
     * TODO: Use demo::Uart instead
     */
    int fileDescriptors_;

    /**
     * Component-wise multiplier that is applied to all measurements before the values are returned. Defaults to 1.0.
     */
    arma::Col<double>::fixed<3> sensorAdjustment_;
  };
}
