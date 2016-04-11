#pragma once

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/linearActuators.hpp"

namespace demo {
  class StewartPlatform {
   public:
    explicit StewartPlatform(
        LinearActuators linearActuators,
        arma::Mat<double>::fixed<3, 6> baseJointsPosition,
        arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition,
        arma::Row<double>::fixed<6> actuatorsMinimalLength,
        arma::Row<double>::fixed<6> actuatorsMaximalLength);

    void setEndEffectorPose(
        const arma::Col<double>::fixed<6>& endEffectorPose);

    void setRelativeEndEffectorPose(
        const arma::Col<double>::fixed<6>& endEffectorPose);

   protected:
    LinearActuators linearActuators_;

    arma::Mat<double>::fixed<3, 6> baseJointsPosition_;
    arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition_;
    arma::Row<double>::fixed<6> actuatorsMinimalLength_;
    arma::Row<double>::fixed<6> actuatorsMaximalLength_;
  };
}