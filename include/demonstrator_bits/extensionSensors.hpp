#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

namespace demo {

  /**
   * Part of `demo::LinearActuators`.
   *
   * Represents an array of extension sensors that are attached to [PRODUCT NAME HERE][1] linear actuators. Sensors can be queried for its current extension through SPI.
   *
   * [1]: http://INSERT-PRODUCT-PAGE-HERE
   */
  class ExtensionSensors {
   public:
    /**
     * The number of extension sensors that are managed.
     */
    const std::size_t numberOfSensors_;

    explicit ExtensionSensors(
        const std::vector<unsigned int>& channels);

    /**
     * Query all actuators for their current extension.
     */
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
    /**
     *
     */
    const std::vector<unsigned int> channels_;

    std::vector<double> minimalMeasureableExtension_;
    std::vector<double> maximalMeasureableExtension_;

    /**
     * Every measurement is multiplied with this number before it is returned. Use it to adjust results to your environment.
     */
    std::vector<double> sensorAdjustments_;
  };
}
