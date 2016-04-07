#pragma once

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/linearActuators.hpp"

namespace demo {
  class StewartPlatform {
   public:
    explicit StewartPlatform(
        LinearActuators linearActuators);
    
    void setEndEffectorPose(
        const arma::Col<double>::fixed<6>& endEffectorPose);
    
    void setRelativeEndEffectorPose(
        const arma::Col<double>::fixed<6>& endEffectorPose);
      
   protected:
    LinearActuators linearActuators_;
  };
}