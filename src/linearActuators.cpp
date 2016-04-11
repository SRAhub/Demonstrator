#include "demonstrator_bits/linearActuators.hpp"

// C++ standard library
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ratio>
#include <stdexcept>
#include <vector>

namespace demo {
  LinearActuators::LinearActuators(
      ServoControllers servoControllers,
      ExtensionSensors extensionSensors)
      : numberOfActuators_(servoControllers.numberOfControllers_),
        servoControllers_(std::move(servoControllers)),
        extensionSensors_(std::move(extensionSensors)) {
    if (servoControllers_.numberOfControllers_ != extensionSensors_.numberOfSensors_) {
      throw std::logic_error("LinearActuators: The number of controllers must be equal to the number of sensors.");
    }
  }

  void LinearActuators::setExtensions(
      const arma::Row<double>& extensions,
      const arma::Row<double>& speeds) {
    stopReachExtension_ = false;
    reachExtension(extensions, speeds);
    // reachExtensionThread_ = std::thread([=]{reachExtension(extensions, speeds);});
  }

  arma::Row<double> LinearActuators::getExtensions() {
    return extensionSensors_.measure();
  }

  void LinearActuators::reachExtension(
      const arma::Row<double>& extensions,
      const arma::Row<double>& maximalSpeeds) {
    arma::Row<double> currentExtension = arma::clamp(extensionSensors_.measure(), minimalExtension_, maximalExtension_);

    bool hasReachedPosition = arma::all(arma::abs(extensions - currentExtension) <= maximalExtensionDeviation_);

    std::vector<bool> forwards = {false, false, false, false, false, false};
    arma::Row<double> speeds = maximalSpeeds;
    while (!hasReachedPosition && !stopReachExtension_) {
      const arma::Row<double>& deviations = currentExtension - extensions;
      for (std::size_t n = 0; n < numberOfActuators_; ++n) {
        if (std::abs(deviations(n)) <= maximalExtensionDeviation_) {
          speeds(n) = 0.0;
        } else {
          speeds(n) = maximalSpeeds(n);

          if (deviations(n) > 0) {
            forwards.at(n) = false;
          } else {
            forwards.at(n) = true;
          }
        }
      }

      servoControllers_.run(forwards, speeds);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      currentExtension = extensionSensors_.measure();

      hasReachedPosition = arma::all(arma::abs(extensions - currentExtension) <= maximalExtensionDeviation_);
    }

    servoControllers_.stop();
  }

  void LinearActuators::setMinimalExtension(
      const double minimalExtension) {
    if (!std::isfinite(minimalExtension)) {
      throw std::domain_error("LinearActuators.setMinimalExtension: The minimal extension must be finite.");
    }

    minimalExtension_ = minimalExtension;
  }

  double LinearActuators::getMinimalExtension() const {
    return minimalExtension_;
  }

  void LinearActuators::setMaximalExtension(
      const double maximalExtension) {
    if (!std::isfinite(maximalExtension)) {
      throw std::domain_error("LinearActuators.setMaximalExtension: The maximal extension must be finite.");
    }

    maximalExtension_ = maximalExtension;
  }

  double LinearActuators::getMaximalExtension() const {
    return maximalExtension_;
  }

  void LinearActuators::setMaximalExtensionDeviation(
      const double maximalExtensionDeviation) {
    if (!std::isfinite(maximalExtensionDeviation)) {
      throw std::domain_error("LinearActuators.setMaximalExtensionDeviation: The maximal extension deviation must be finite.");
    }

    maximalExtensionDeviation_ = maximalExtensionDeviation;
  }

  double LinearActuators::getMaximalExtensionDeviation() const {
    return maximalExtensionDeviation_;
  }
}