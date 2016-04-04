#include "demonstrator_bits/linearActuators.hpp"

namespace demo {
  void LinearActuators::setMinimalExtensions(
      const std::vector<double>& minimalExtensions) {
        
  }
  
  std::vector<double> LinearActuators::getMinimalExtensions() const {
    return minimalExtensions_;
  }

  void LinearActuators::setMaximalExtensions(
      const std::vector<double>& maximalExtensions) {
    return 
  }
  
  std::vector<double> LinearActuators::getMaximalExtensions() const {
        
  }

  void LinearActuators::setMinimalSpeeds(
      const std::vector<double>& minimalSpeeds) {
    return 
  }
  
  std::vector<double> LinearActuators::getMinimalSpeeds() const {
        
  }

  void LinearActuators::setMaximalSpeeds(
      const std::vector<double>& maximalSpeeds) {
    maximalSpeeds_ = maximalSpeeds;
  }
  
  std::vector<double> LinearActuators::getMaximalSpeeds() const {
    return maximalSpeeds_;
  }
    
  void LinearActuators::setExtensionCorrections(
      const std::vector<double>& extensionCorrections) {
    extensionCorrections_ = extensionCorrections;
  }
  
  void LinearActuators::getExtensionCorrections() const {
    return extensionCorrections_;
  }
  
  void LinearActuators::setSpeedCorrections(
      const std::vector<double>& speedCorrections) {
        
  }
      
  void LinearActuators::getSpeedCorrections() const {
    speedCorrections_ = speedCorrections;
  }
}