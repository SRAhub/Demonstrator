#include "demonstrator_bits/stewartPlatform.hpp"

// C++ standard library
#include <algorithm>
#include <stdexcept>

namespace demo {
  StewartPlatform::StewartPlatform(
      LinearActuators linearActuators,
      arma::Mat<double>::fixed<3, 6> baseJointsPosition,
      arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition,
      arma::Row<double>::fixed<6> actuatorsMinimalLength,
      arma::Row<double>::fixed<6> actuatorsMaximalLength)
      : linearActuators_(std::move(linearActuators)),
        baseJointsPosition_(baseJointsPosition),
        endEffectorJointsRelativePosition_(endEffectorJointsRelativePosition) {
    if (linearActuators_.numberOfActuators_ != 6) {
      throw std::invalid_argument("LinearActuators: A Stewart platform must have 6 actuators.");
    }
  }

  void StewartPlatform::setEndEffectorPose(
      const arma::Col<double>::fixed<6>& endEffectorPose) {
    arma::Row<double>::fixed<6> extensions = arma::sqrt(arma::sum(arma::square(static_cast<arma::Mat<double>>(endEffectorJointsRelativePosition_ + endEffectorPose.head(3) - baseJointsPosition_))));

    linearActuators_.setExtensions(extensions, arma::ones<arma::Row<double>>(6));
  }

  void StewartPlatform::setRelativeEndEffectorPose(
      const arma::Col<double>::fixed<6>& endEffectorPose) {
  }
}