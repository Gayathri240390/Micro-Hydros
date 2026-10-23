#include <unity.h>

#include <limits>

#include "validator.h"

namespace {
MeasurementData validMeasurement() {
    MeasurementData measurement{};
    measurement.airInsideC = 22.0f;
    measurement.airOutsideC = 15.0f;
    measurement.waterC = 20.0f;
    measurement.humidityInsidePct = 55.0f;
    return measurement;
}
}

void test_accepts_in_range_values() {
    TEST_ASSERT_TRUE(Validator::isValid(validMeasurement()));
}

void test_accepts_inclusive_boundaries() {
    MeasurementData measurement = validMeasurement();
    measurement.airInsideC = -10.0f;
    measurement.airOutsideC = 60.0f;
    measurement.waterC = 0.0f;
    measurement.humidityInsidePct = 100.0f;
    TEST_ASSERT_TRUE(Validator::isValid(measurement));
}

void test_rejects_out_of_range_humidity() {
    MeasurementData measurement = validMeasurement();
    measurement.humidityInsidePct = 100.1f;
    TEST_ASSERT_FALSE(Validator::isValid(measurement));
}

void test_rejects_non_finite_temperature() {
    MeasurementData measurement = validMeasurement();
    measurement.waterC = std::numeric_limits<float>::quiet_NaN();
    TEST_ASSERT_FALSE(Validator::isValid(measurement));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_accepts_in_range_values);
    RUN_TEST(test_accepts_inclusive_boundaries);
    RUN_TEST(test_rejects_out_of_range_humidity);
    RUN_TEST(test_rejects_non_finite_temperature);
    return UNITY_END();
}