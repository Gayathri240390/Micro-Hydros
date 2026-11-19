#include <unity.h>

#include "alarm.h"
#include "config.h"

void setUp() {}
void tearDown() {}

namespace {
MeasurementData normalMeasurement() {
    MeasurementData m;
    m.airInsideC = 23.0f;
    m.airOutsideC = 20.0f;
    m.waterC = 21.0f;
    m.humidityInsidePct = 60.0f;
    return m;
}
}

void test_inside_air_alarm_boundaries() {
    MeasurementData m = normalMeasurement();
    m.airInsideC = config::kAirInsideMinC - 0.1f;
    TEST_ASSERT_EQUAL_UINT8(kAlarmAirInsideLow, AlarmEvaluator::evaluate(m));
    m.airInsideC = config::kAirInsideMinC;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.airInsideC = config::kAirInsideMaxC;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.airInsideC = config::kAirInsideMaxC + 0.1f;
    TEST_ASSERT_EQUAL_UINT8(kAlarmAirInsideHigh, AlarmEvaluator::evaluate(m));
}

void test_water_alarm_boundaries() {
    MeasurementData m = normalMeasurement();
    m.waterC = config::kWaterMinC - 0.1f;
    TEST_ASSERT_EQUAL_UINT8(kAlarmWaterLow, AlarmEvaluator::evaluate(m));
    m.waterC = config::kWaterMinC;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.waterC = config::kWaterMaxC;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.waterC = config::kWaterMaxC + 0.1f;
    TEST_ASSERT_EQUAL_UINT8(kAlarmWaterHigh, AlarmEvaluator::evaluate(m));
}

void test_humidity_alarm_boundaries() {
    MeasurementData m = normalMeasurement();
    m.humidityInsidePct = config::kHumidityMinPct - 0.1f;
    TEST_ASSERT_EQUAL_UINT8(kAlarmHumidityLow, AlarmEvaluator::evaluate(m));
    m.humidityInsidePct = config::kHumidityMinPct;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.humidityInsidePct = config::kHumidityMaxPct;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.humidityInsidePct = config::kHumidityMaxPct + 0.1f;
    TEST_ASSERT_EQUAL_UINT8(kAlarmHumidityHigh, AlarmEvaluator::evaluate(m));
}

void test_faults_suppress_only_affected_alarms() {
    MeasurementData m = normalMeasurement();
    m.airInsideC = config::kAirInsideMaxC + 1.0f;
    m.waterC = config::kWaterMinC - 1.0f;
    m.humidityInsidePct = config::kHumidityMaxPct + 1.0f;
    m.readFailures = kWater;
    m.implausible = kAirInside;
    TEST_ASSERT_EQUAL_UINT8(kAlarmHumidityHigh, AlarmEvaluator::evaluate(m));
    m.implausible = kHumidityInside;
    m.readFailures = kAirInside | kWater;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
}

void test_outside_air_does_not_raise_alarm() {
    MeasurementData m = normalMeasurement();
    m.airOutsideC = -10.0f;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
    m.airOutsideC = 45.0f;
    TEST_ASSERT_EQUAL_UINT8(0, AlarmEvaluator::evaluate(m));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_inside_air_alarm_boundaries);
    RUN_TEST(test_water_alarm_boundaries);
    RUN_TEST(test_humidity_alarm_boundaries);
    RUN_TEST(test_faults_suppress_only_affected_alarms);
    RUN_TEST(test_outside_air_does_not_raise_alarm);
    return UNITY_END();
}
