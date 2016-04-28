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

// Demonstrator
#include "demonstrator_bits/mantella.hpp"

namespace demo {
  StewartPlatform::StewartPlatform(
      LinearActuators&& linearActuators,
      AttitudeSensors&& attitudeSensors,
      const arma::Mat<double>::fixed<3, 6>& baseJointsPosition,
      const arma::Mat<double>::fixed<3, 6>& endEffectorJointsRelativePosition,
      const arma::Col<double>::fixed<6>& minimalEndEffectorPose,
      const arma::Col<double>::fixed<6>& maximalEndEffectorPose)
      : linearActuators_(std::move(linearActuators)),
        attitudeSensors_(std::move(attitudeSensors)),
        baseJointsPosition_(baseJointsPosition),
        endEffectorJointsRelativePosition_(endEffectorJointsRelativePosition),
        minimalEndEffectorPose_(minimalEndEffectorPose),
        maximalEndEffectorPose_(maximalEndEffectorPose) {
    if (linearActuators_.numberOfActuators_ != 6) {
      throw std::invalid_argument("StewartPlatform: A Stewart platform must have 6 actuators.");
    } else if (attitudeSensors_.numberOfSensors_ != 3) {
      throw std::invalid_argument("StewartPlatform: The Stewart platform must have 3 attitudes sensors.");
    }
    
    attitudeSensors_.runAsynchronous();
  }

  StewartPlatform::StewartPlatform(
      StewartPlatform&& stewartPlatform)
      : StewartPlatform(std::move(stewartPlatform.linearActuators_), std::move(stewartPlatform.attitudeSensors_), stewartPlatform.baseJointsPosition_, stewartPlatform.endEffectorJointsRelativePosition_, stewartPlatform.minimalEndEffectorPose_, stewartPlatform.maximalEndEffectorPose_) {
  }

  StewartPlatform& StewartPlatform::operator=(
      StewartPlatform&& stewartPlatform) {
    if (arma::any(arma::vectorise(arma::abs(baseJointsPosition_ - stewartPlatform.baseJointsPosition_) > 0))) {
      throw std::invalid_argument("StewartPlatform.operator=: The base joints positions must be equal.");
    } else if (arma::any(arma::vectorise(arma::abs(endEffectorJointsRelativePosition_ - stewartPlatform.endEffectorJointsRelativePosition_) > 0))) {
      throw std::invalid_argument("StewartPlatform.operator=: The relative end-effector joints positions must be equal.");
    } else if (arma::any(arma::abs(minimalEndEffectorPose_ - stewartPlatform.minimalEndEffectorPose_) > 0)) {
      throw std::invalid_argument("StewartPlatform.operator=: The minimal end-effector poses must be equal.");
    } else if (arma::any(arma::abs(maximalEndEffectorPose_ - stewartPlatform.maximalEndEffectorPose_) > 0)) {
      throw std::invalid_argument("StewartPlatform.operator=: The maximal end-effector poses must be equal.");
    }

    linearActuators_ = std::move(stewartPlatform.linearActuators_);
    attitudeSensors_ = std::move(stewartPlatform.attitudeSensors_);
    
    attitudeSensors_.runAsynchronous();
    
    return *this;
  }

  void StewartPlatform::setEndEffectorPose(
      const arma::Col<double>::fixed<6>& endEffectorPose) {
    if (!endEffectorPose.is_finite()) {
      throw std::domain_error("StewartPlatform.setEndEffectorPose: All end-effector poses must be finite.");
    }
    
    limitedEndEffectorPose_ = arma::min(arma::max(endEffectorPose, minimalEndEffectorPose_), maximalEndEffectorPose_);

    arma::Row<double>::fixed<6> extensions;
    const arma::Mat<double>::fixed<3, 3>& endeEffectorRotation = mant::rotationMatrix3D(limitedEndEffectorPose_(3), limitedEndEffectorPose_(4), limitedEndEffectorPose_(5));
    for (std::size_t n = 0; n < linearActuators_.numberOfActuators_; ++n) {
      extensions(n) = arma::norm(baseJointsPosition_.col(n) - (endeEffectorRotation * endEffectorJointsRelativePosition_.col(n) + limitedEndEffectorPose_.head(3)));
    }

    if (arma::all(extensions >= linearActuators_.minimalAllowedExtension_) && arma::all(extensions <= linearActuators_.maximalAllowedExtension_)) {
      linearActuators_.setExtensions(extensions, extensions / arma::max(extensions));
    }
  }

  arma::Col<double>::fixed<6> StewartPlatform::getEndEffectorPose() {
    mant::OptimisationProblem endEffectorPose;
    endEffectorPose.setObjectiveFunction([](
        const arma::Col<double>& parameter) {
      arma::Row<double>::fixed<6> extensions;
      for (std::size_t n = 0; n < linearActuators_.numberOfActuators_; ++n) {
        extensions(n) = arma::norm(baseJointsPosition_.col(n) - (endEffectorJointsRelativePosition_.col(n) + parameter.head(3)));
      }
      
      return arma::accu(arma::abs(extensions - linearActuators_.getExtensions());
    });
    
    mant::RandomSearch randomSearch;
    randomSearch.setAcceptableObjectiveValue(linearActuators_.getMaximalExtensionDeviation());
    randomSearch.setMaximalDuration(std::chrono::milliseconds(200));
    randomSearch.optimise(endEffectorPose);
    
    return randomSearch.getBestParameter();
  }

  bool StewartPlatform::waitTillEndEffectorPoseIsReached(
      const std::chrono::microseconds timeout) {
    return linearActuators_.waitTillExtensionIsReached(timeout);
  }
}