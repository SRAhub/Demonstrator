#pragma once

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/linearActuators.hpp"
#include "demonstrator_bits/sensors/attitudeSensors.hpp"

namespace demo {
  class StewartPlatform {
   public:
    explicit StewartPlatform(
        LinearActuators&& linearActuators,
        AttitudeSensors&& attitudeSensors,
        arma::Mat<double>::fixed<3, 6> baseJointsPosition,
        arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition,
        arma::Row<double>::fixed<6> actuatorsMinimalLength,
        arma::Row<double>::fixed<6> actuatorsMaximalLength);

    explicit StewartPlatform(
        StewartPlatform&& stewartPlatform);

    StewartPlatform& operator=(
        StewartPlatform&& stewartPlatform);

    StewartPlatform(StewartPlatform&) = delete;
    StewartPlatform& operator=(StewartPlatform&) = delete;

    void setEndEffectorPose(
        const arma::Col<double>::fixed<6>& endEffectorPose);

    arma::Col<double>::fixed<6> getEndEffectorPose();

   protected:
    LinearActuators linearActuators_;
    AttitudeSensors attitudeSensors_;

    arma::Mat<double>::fixed<3, 6> baseJointsPosition_;
    arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition_;
    arma::Row<double>::fixed<6> actuatorsMinimalLength_;
    arma::Row<double>::fixed<6> actuatorsMaximalLength_;
  };
}