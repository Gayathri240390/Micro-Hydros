#include <Arduino.h>

#include "sensor_manager.h"

bool SensorManager::begin() {
    return true;
}

MeasurementData SensorManager::read(std::uint32_t timestampMs) const {
    MeasurementData measurement{};
    measurement.timestampMs = timestampMs;

#if USE_SIMULATED_SENSORS
    const float phase = static_cast<float>((timestampMs / 1000U) % 20U);
    measurement.airInsideC = 22.0f + phase * 0.05f;
    measurement.airOutsideC = 18.0f + phase * 0.03f;
    measurement.waterC = 20.0f + phase * 0.02f;
    measurement.humidityInsidePct = 56.0f + phase * 0.1f;
#endif

    return measurement;
}