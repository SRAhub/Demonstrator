#pragma once

// Armadillo
#include <armadillo>

namespace demo {
  class AttitudeSensor {
   public:
    AttitudeSensor();

    arma::Col<double>::fixed<3> getAttitude() const;

    void reset();

    void setSensorAdjustment(
        const arma::Col<double>::fixed<3>& sensorAdjustment);
    arma::Col<double>::fixed<3> getSensorAdjustment() const;

   protected:
    int fileDescriptors_;

    arma::Col<double>::fixed<3> sensorAdjustment_;
  };
}
