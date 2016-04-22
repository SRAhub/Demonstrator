#pragma once

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/linearActuators.hpp"
#include "demonstrator_bits/sensors/attitudeSensors.hpp"

namespace demo {
  class StewartPlatform {
   public:
    const arma::Mat<double>::fixed<3, 6> baseJointsPosition_;
    const arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition_;
    const arma::Col<double>::fixed<6> minimalEndEffectorPose_;
    const arma::Col<double>::fixed<6> maximalEndEffectorPose_;
    
    explicit StewartPlatform(
        LinearActuators&& linearActuators,
        AttitudeSensors&& attitudeSensors,
        const arma::Mat<double>::fixed<3, 6>& baseJointsPosition,
        const arma::Mat<double>::fixed<3, 6>& endEffectorJointsRelativePosition,
        const arma::Col<double>::fixed<6>& minimalEndEffectorPose,
        const arma::Col<double>::fixed<6>& maximalEndEffectorPose);

    explicit StewartPlatform(
        StewartPlatform&& stewartPlatform);

    StewartPlatform& operator=(
        StewartPlatform&& stewartPlatform);

    StewartPlatform(StewartPlatform&) = delete;
    StewartPlatform& operator=(StewartPlatform&) = delete;

    void setEndEffectorPose(
        const arma::Col<double>::fixed<6>& endEffectorPose);

    arma::Col<double>::fixed<6> getEndEffectorPose();

    bool waitTillEndEffectorPoseIsReached(
        const std::chrono::microseconds timeout);

   protected:
    LinearActuators linearActuators_;
    AttitudeSensors attitudeSensors_;
  };
}