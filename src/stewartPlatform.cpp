#include "demonstrator_bits/stewartPlatform.hpp"

// C++ standard library
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <stdexcept>
#include <vector>
// IWYU pragma: no_include <ext/alloc_traits.h>

// Mantella
#include <mantella>

namespace demo {
  StewartPlatform::StewartPlatform(
      LinearActuators&& linearActuators,
      AttitudeSensors&& attitudeSensors,
      arma::Mat<double>::fixed<3, 6> baseJointsPosition,
      arma::Mat<double>::fixed<3, 6> endEffectorJointsRelativePosition,
      arma::Row<double>::fixed<6> actuatorsMinimalLength,
      arma::Row<double>::fixed<6> actuatorsMaximalLength)
      : linearActuators_(std::move(linearActuators)),
        attitudeSensors_(std::move(attitudeSensors)),
        baseJointsPosition_(baseJointsPosition),
        endEffectorJointsRelativePosition_(endEffectorJointsRelativePosition),
        actuatorsMinimalLength_(actuatorsMinimalLength),
        actuatorsMaximalLength_(actuatorsMaximalLength) {
    if (linearActuators_.numberOfActuators_ != 6) {
      throw std::invalid_argument("StewartPlatform: A Stewart platform must have 6 actuators.");
    } else if (attitudeSensors_.numberOfSensors_ != 3) {
      throw std::invalid_argument("StewartPlatform: The Stewart platform must have 3 attitudes sensors.");
    }
    
    attitudeSensors_.runAsynchronous();
  }

  StewartPlatform::StewartPlatform(
      StewartPlatform&& stewartPlatform)
      : StewartPlatform(std::move(stewartPlatform.linearActuators_), std::move(stewartPlatform.attitudeSensors_), stewartPlatform.baseJointsPosition_, stewartPlatform.endEffectorJointsRelativePosition_, stewartPlatform.actuatorsMinimalLength_, stewartPlatform.actuatorsMaximalLength_) {
  }

  StewartPlatform& StewartPlatform::operator=(
      StewartPlatform&& stewartPlatform) {
    linearActuators_ = std::move(stewartPlatform.linearActuators_);
    attitudeSensors_ = std::move(stewartPlatform.attitudeSensors_);
    baseJointsPosition_ = stewartPlatform.baseJointsPosition_;
    endEffectorJointsRelativePosition_ = stewartPlatform.endEffectorJointsRelativePosition_;
    actuatorsMinimalLength_ = stewartPlatform.actuatorsMinimalLength_;
    actuatorsMaximalLength_ = stewartPlatform.actuatorsMaximalLength_;

    attitudeSensors_.runAsynchronous();
    
    return *this;
  }

  void StewartPlatform::setEndEffectorPose(
      const arma::Col<double>::fixed<6>& endEffectorPose) {
    if (!endEffectorPose.is_finite()) {
      throw std::domain_error("StewartPlatform.setEndEffectorPose: All end-effector poses must be finite.");
    }

    arma::Row<double>::fixed<6> extensions;
    const arma::Mat<double>::fixed<3, 3>& endeEffectorRotation = mant::rotationMatrix3D(endEffectorPose(3), endEffectorPose(4), endEffectorPose(5));
    for (std::size_t n = 0; n < linearActuators_.numberOfActuators_; ++n) {
      extensions(n) = arma::norm(baseJointsPosition_.col(n) - endeEffectorRotation * endEffectorJointsRelativePosition_.col(n) + endEffectorPose.head(3));
    }
    extensions -= actuatorsMinimalLength_;
    extensions /= actuatorsMaximalLength_ - actuatorsMinimalLength_;

    // TODO intermediate extensions

    if (arma::all(extensions >= linearActuators_.minimalExtension_) && arma::all(extensions <= linearActuators_.maximalExtension_)) {
      linearActuators_.setExtensions(extensions, extensions / arma::max(extensions));
    }
  }

  arma::Col<double>::fixed<6> StewartPlatform::getEndEffectorPose() {
    const arma::Row<double>::fixed<3>& attitudes = attitudeSensors_.measure();
    const arma::Row<double>::fixed<6>& extensions = linearActuators_.getExtensions();

    const arma::Mat<double>::fixed<3, 3>& endEffectorRotation = mant::rotationMatrix3D(attitudes(0), attitudes(1), attitudes(2));

    // Given the end-effector's attitudes, the remaining end-effector pose (its x,y,z-translation) is already fully described by the intersection of three actuators.
    arma::Mat<double>::fixed<3, 3> baseJointsShiftedPosition;
    for (std::size_t n = 0; n < 3; ++n) {
      baseJointsShiftedPosition.col(n) = baseJointsPosition_.col(n) - endEffectorRotation * endEffectorJointsRelativePosition_.col(n);
    }

    const double distanceBetweenSpheres = arma::norm(baseJointsShiftedPosition.col(0) - baseJointsShiftedPosition.col(1));
    const double relativeDistanceToIntersection = 0.5 + (std::pow(extensions(0), 2.0) - std::pow(extensions(1), 2.0)) / std::pow(distanceBetweenSpheres, 2.0);

    const arma::Col<double>::fixed<3>& circleCenter = baseJointsShiftedPosition.col(0) + relativeDistanceToIntersection * (baseJointsShiftedPosition.col(1) - baseJointsShiftedPosition.col(0));
    const double circleRadius = std::sqrt(std::pow(extensions(0), 2.0) - std::pow(distanceBetweenSpheres * relativeDistanceToIntersection, 2.0));
    const arma::Col<double>::fixed<3>& circleNormal = (baseJointsShiftedPosition.col(0) - baseJointsShiftedPosition.col(1)) / distanceBetweenSpheres;

    const std::vector<arma::Col<double>::fixed<3>>& intersections = mant::circleSphereIntersections(circleCenter, circleRadius, circleNormal, baseJointsShiftedPosition.col(2), extensions(2));

    if (intersections.size() < 1) {
      throw std::runtime_error("StewartPlatform.getEndEffectorPose: ");
    }

    if (intersections.at(0)(2) > 0) {
      return arma::join_cols(intersections.at(0), attitudes.t());
    } else {
      return arma::join_cols(intersections.at(1), attitudes.t());
    }
  }

  bool StewartPlatform::waitTillEndEffectorPoseIsReached(
      const std::chrono::microseconds timeout) {
    return linearActuators_.waitTillExtensionIsReached(timeout);
  }
}