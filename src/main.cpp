#include <Arduino.h>

#include "communication.h"
#include "sensor_manager.h"
#include "validator.h"

namespace {
constexpr std::uint32_t measurementIntervalMs = 5000;
SensorManager sensorManager;
Communication communication;
std::uint32_t lastMeasurementMs = 0;
}

void setup() {
    Serial.begin(115200);
    sensorManager.begin();
    communication.begin();
    Serial.println("MicroHydros simulated prototype started");
}

void loop() {
    communication.loop();
    const std::uint32_t now = millis();
    if (now - lastMeasurementMs < measurementIntervalMs) {
        return;
    }

    lastMeasurementMs = now;
    MeasurementData measurement = sensorManager.read(now);
    measurement.valid = Validator::isValid(measurement);
    communication.send(measurement);
}