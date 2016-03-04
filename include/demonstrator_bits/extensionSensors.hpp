#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

namespace demo {
  class ExtensionSensors {
   public:
    /**
     * The number of extension sensors that are managed.
     */
    const std::size_t numberOfSensors_;

    explicit ExtensionSensors(
        const std::vector<unsigned int>& channels);

    std::vector<double> getExtensions();

    void setMinimalMeasureableExtension(
        const std::vector<double>& minimalMeasureableExtension);
    std::vector<double> getMinimalMeasureableExtension() const;

    void setMaximalMeasureableExtension(
        const std::vector<double>& maximalMeasureableExtension);
    std::vector<double> getMaximalMeasureableExtension() const;

    void setSensorAdjustments(
        const std::vector<double>& sensorAdjustments);
    std::vector<double> getSensorAdjustments() const;

   protected:
    const std::vector<unsigned int> channels_;

    std::vector<double> minimalMeasureableExtension_;
    std::vector<double> maximalMeasureableExtension_;

    std::vector<double> sensorAdjustments_;
  };
}
