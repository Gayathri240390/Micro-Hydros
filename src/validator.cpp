#include <cmath>

#include "validator.h"

bool Validator::isValid(const MeasurementData& measurement) {
    return std::isfinite(measurement.airInsideC) &&
           std::isfinite(measurement.airOutsideC) &&
           std::isfinite(measurement.waterC) &&
           std::isfinite(measurement.humidityInsidePct) &&
           measurement.airInsideC >= -10.0f && measurement.airInsideC <= 60.0f &&
           measurement.airOutsideC >= -30.0f && measurement.airOutsideC <= 60.0f &&
           measurement.waterC >= 0.0f && measurement.waterC <= 50.0f &&
           measurement.humidityInsidePct >= 0.0f && measurement.humidityInsidePct <= 100.0f;
}