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
      const double minimalExtension,
      const double maximalExtension)
      : numberOfActuators_(servoControllers.numberOfControllers_),
        servoControllers_(std::move(servoControllers)),
        extensionSensors_(std::move(extensionSensors)),
        minimalExtension_(minimalExtension),
        maximalExtension_(maximalExtension) {
    if (servoControllers_.numberOfControllers_ != extensionSensors_.numberOfSensors_) {
      throw std::logic_error("LinearActuators: The number of controllers must be equal to the number of sensors.");
    }
  }

  LinearActuators::LinearActuators(
      LinearActuators&& linearActuators)
      : LinearActuators(std::move(linearActuators.servoControllers_), std::move(linearActuators.extensionSensors_), linearActuators.minimalExtension_, linearActuators.maximalExtension_) {
  }

  LinearActuators& LinearActuators::operator=(
      LinearActuators&& linearActuators) {
    if (numberOfActuators_ != linearActuators.numberOfActuators_) {
      throw std::invalid_argument("LinearActuators.operator=: The number of actuators must be equal.");
    } else if (std::abs(minimalExtension_ -  linearActuators.minimalExtension_) > 0) {
      throw std::invalid_argument("LinearActuators.operator=: The minimal extensions must be equal.");
    } else if (std::abs(maximalExtension_ -  linearActuators.maximalExtension_) > 0) {
      throw std::invalid_argument("LinearActuators.operator=: The minimal extensions must be equal.");
    } 
        
    servoControllers_ = std::move(linearActuators.servoControllers_);
    extensionSensors_ = std::move(linearActuators.extensionSensors_);

    return *this;
  }
  
  LinearActuators::~LinearActuators() {
    if (killReachExtensionThread_ == false) {
      killReachExtensionThread_ = true;
      reachExtensionThread_.join();
    }
  }

  void LinearActuators::setExtensions(
      const arma::Row<double>& extensions,
      const arma::Row<double>& speeds) {
    std::cout << "LinearActuators::setExtensions" << std::endl;
    if (killReachExtensionThread_ == false) {
      std::cout << "killReachExtensionThread_ = true;" << std::endl;
      killReachExtensionThread_ = true;
      reachExtensionThread_.join();
      std::cout << "reachExtensionThread_.join();" << std::endl;
    }
    
    killReachExtensionThread_ = false;
    std::cout << "std::thread " << extensions << speeds << std::endl;
    reachExtensionThread_ = std::thread(&LinearActuators::reachExtension, this, extensions, speeds);
  }

  arma::Row<double> LinearActuators::getExtensions() {
    return extensionSensors_.measure();
  }

  void LinearActuators::reachExtension(
      const arma::Row<double>& extensions,
      const arma::Row<double>& maximalSpeeds) {
    std::cout << "LinearActuators::reachExtension" << std::endl;
    std::cout << "reachExtension " << extensions << maximalSpeeds << std::endl;
    arma::Row<double> limitedExtensions = arma::clamp(extensions, minimalExtension_, maximalExtension_);
    arma::Row<double> currentExtension = extensionSensors_.measure();
    std::cout << "reachExtension.currentExtension " << currentExtension << std::endl;

    bool hasReachedPosition = arma::all(arma::abs(limitedExtensions - currentExtension) <= maximalExtensionDeviation_);

    std::vector<bool> forwards(numberOfActuators_, false);
    arma::Row<double> speeds = maximalSpeeds;
    while (!hasReachedPosition && !killReachExtensionThread_) {
      const arma::Row<double>& deviations = currentExtension - limitedExtensions;
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

      hasReachedPosition = arma::all(arma::abs(limitedExtensions - currentExtension) <= maximalExtensionDeviation_);
    }

    servoControllers_.stop();
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

  ServoControllers& LinearActuators::getServoControllers() {
    return servoControllers_;
  }

  ExtensionSensors& LinearActuators::getExtensionSensors() {
    return extensionSensors_;
  }
}
