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
      ServoControllers&& servoControllers,
      ExtensionSensors&& extensionSensors,
      const double minimalAllowedExtension,
      const double maximalAllowedExtension)
      : numberOfActuators_(servoControllers.numberOfControllers_),
        servoControllers_(std::move(servoControllers)),
        extensionSensors_(std::move(extensionSensors)),
        minimalAllowedExtension_(minimalAllowedExtension),
        maximalAllowedExtension_(maximalAllowedExtension) {
    if (servoControllers_.numberOfControllers_ != extensionSensors_.numberOfSensors_) {
      throw std::logic_error("LinearActuators: The number of controllers must be equal to the number of sensors.");
    }
    
    setAcceptableExtensionDeviation(0.0);
  }

  LinearActuators::LinearActuators(
      LinearActuators&& linearActuators)
      : LinearActuators(std::move(linearActuators.servoControllers_), std::move(linearActuators.extensionSensors_), linearActuators.minimalAllowedExtension_, linearActuators.maximalAllowedExtension_) {
    setAcceptableExtensionDeviation(linearActuators.acceptableExtensionDeviation_);
  }

  LinearActuators& LinearActuators::operator=(
      LinearActuators&& linearActuators) {
    if (numberOfActuators_ != linearActuators.numberOfActuators_) {
      throw std::invalid_argument("LinearActuators.operator=: The number of actuators must be equal.");
    } else if (std::abs(minimalAllowedExtension_ -  linearActuators.minimalAllowedExtension_) > 0) {
      throw std::invalid_argument("LinearActuators.operator=: The minimal allowed extensions must be equal.");
    } else if (std::abs(maximalAllowedExtension_ -  linearActuators.maximalAllowedExtension_) > 0) {
      throw std::invalid_argument("LinearActuators.operator=: The minimal allowed extensions must be equal.");
    }
        
    servoControllers_ = std::move(linearActuators.servoControllers_);
    extensionSensors_ = std::move(linearActuators.extensionSensors_);
    
    setAcceptableExtensionDeviation(linearActuators.acceptableExtensionDeviation_);

    return *this;
  }
  
  LinearActuators::~LinearActuators() {
    if (reachExtensionThread_.joinable()) {
      killReachExtensionThread_ = true;
      reachExtensionThread_.join();
    }
  }

  void LinearActuators::setExtensions(
      const arma::Row<double>& extensions,
      const arma::Row<double>& speeds) {
    if (reachExtensionThread_.joinable()) {
      killReachExtensionThread_ = true;
      reachExtensionThread_.join();
    }
    
    killReachExtensionThread_ = false;
    reachExtensionThread_ = std::thread(&LinearActuators::reachExtension, this, extensions, speeds);
  }

  arma::Row<double> LinearActuators::getExtensions() {
    return extensionSensors_.measure();
  }

  void LinearActuators::reachExtension(
      const arma::Row<double>& extensions,
      const arma::Row<double>& maximalSpeeds) {
    arma::Row<double> limitedExtensions = arma::clamp(extensions, minimalAllowedExtension_, maximalAllowedExtension_);
    arma::Row<double> currentExtension = extensionSensors_.measure();

    bool hasReachedPosition = arma::all(arma::abs(limitedExtensions - currentExtension) <= acceptableExtensionDeviation_);

    std::vector<bool> forwards(numberOfActuators_, false);
    arma::Row<double> speeds = maximalSpeeds;
    while (!hasReachedPosition && !killReachExtensionThread_) {
      const arma::Row<double>& deviations = currentExtension - limitedExtensions;
      for (std::size_t n = 0; n < numberOfActuators_; ++n) {
        if (std::abs(deviations(n)) <= acceptableExtensionDeviation_) {
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

      hasReachedPosition = arma::all(arma::abs(limitedExtensions - currentExtension) <= acceptableExtensionDeviation_);
    }

    servoControllers_.stop();
    killReachExtensionThread_ = true;
  }

  bool LinearActuators::waitTillExtensionIsReached(
      const std::chrono::microseconds timeout) {
    auto start = std::chrono::steady_clock::now();
    while (killReachExtensionThread_ == false) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(500));
      
      auto end = std::chrono::steady_clock::now();
      if (end - start >= timeout) {
        return false;
      }
    }
    
    return true;
  }

  void LinearActuators::setAcceptableExtensionDeviation(
      const double acceptableExtensionDeviation) {
    if (!std::isfinite(acceptableExtensionDeviation)) {
      throw std::domain_error("LinearActuators.setAcceptableExtensionDeviation: The maximal extension deviation must be finite.");
    }

    acceptableExtensionDeviation_ = acceptableExtensionDeviation;
  }

  double LinearActuators::getMaximalExtensionDeviation() const {
    return acceptableExtensionDeviation_;
  }
}
