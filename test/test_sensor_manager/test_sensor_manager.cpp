#include <unity.h>

void setUp() {}
void tearDown() {}

#include <cmath>

#include "config.h"
#include "hal/simulated_sensors.h"
#include "sensor_manager.h"
#include "simulation_profiles.h"

namespace {

// Minimal fake implementing the HAL directly: proves SensorManager only depends on the
// interfaces, i.e. simulation and real drivers are interchangeable.
class FixedTemperature : public hal::ITemperatureSensor {
public:
    explicit FixedTemperature(float value, bool ok = true) : value_(value), ok_(ok) {}
    bool begin() override { return true; }
    bool readCelsius(float& c) override { c = value_; return ok_; }
    float value_;
    bool ok_;
};

class FixedHumidity : public hal::IHumiditySensor {
public:
    explicit FixedHumidity(float value, bool ok = true) : value_(value), ok_(ok) {}
    bool begin() override { return true; }
    bool readRelativeHumidity(float& p) override { p = value_; return ok_; }
    float value_;
    bool ok_;
};

}  // namespace

void test_all_channels_read_and_valid() {
    FixedTemperature in(23.0f), out(21.0f), water(20.0f);
    FixedHumidity rh(60.0f);
    SensorManager manager(in, out, water, rh);
    const MeasurementData m = manager.read(1000);
    TEST_ASSERT_TRUE(m.valid);
    TEST_ASSERT_EQUAL_UINT8(0, m.readFailures);
    TEST_ASSERT_EQUAL_UINT8(0, m.implausible);
    TEST_ASSERT_EQUAL_UINT8(0, m.alarms);
    TEST_ASSERT_EQUAL_FLOAT(23.0f, m.airInsideC);
    TEST_ASSERT_EQUAL_UINT32(1000, m.timestampMs);
}

void test_sequence_increments() {
    FixedTemperature in(23.0f), out(21.0f), water(20.0f);
    FixedHumidity rh(60.0f);
    SensorManager manager(in, out, water, rh);
    TEST_ASSERT_EQUAL_UINT32(0, manager.read(0).sequence);
    TEST_ASSERT_EQUAL_UINT32(1, manager.read(5000).sequence);
    TEST_ASSERT_EQUAL_UINT32(2, manager.read(10000).sequence);
}

void test_read_failure_marks_only_that_channel() {
    FixedTemperature in(23.0f), out(21.0f), water(20.0f, false);
    FixedHumidity rh(60.0f);
    SensorManager manager(in, out, water, rh);
    const MeasurementData m = manager.read(0);
    TEST_ASSERT_FALSE(m.valid);
    TEST_ASSERT_EQUAL_UINT8(kWater, m.readFailures);
    TEST_ASSERT_EQUAL_UINT8(0, m.implausible);  // a failed read is not double-counted
    TEST_ASSERT_TRUE(std::isnan(m.waterC));
    TEST_ASSERT_EQUAL_FLOAT(23.0f, m.airInsideC);
}

void test_implausible_value_is_flagged_without_alarm() {
    FixedTemperature in(150.0f), out(21.0f), water(20.0f);
    FixedHumidity rh(60.0f);
    SensorManager manager(in, out, water, rh);
    const MeasurementData m = manager.read(0);
    TEST_ASSERT_FALSE(m.valid);
    TEST_ASSERT_EQUAL_UINT8(kAirInside, m.implausible);
    TEST_ASSERT_EQUAL_UINT8(0, m.alarms);  // broken sensor is a fault, not "too hot"
}

void test_threshold_alarms() {
    FixedTemperature in(config::kAirInsideMaxC + 1.0f), out(21.0f), water(config::kWaterMinC - 1.0f);
    FixedHumidity rh(config::kHumidityMaxPct + 1.0f);
    SensorManager manager(in, out, water, rh);
    const MeasurementData m = manager.read(0);
    TEST_ASSERT_TRUE(m.valid);
    TEST_ASSERT_EQUAL_UINT8(kAlarmAirInsideHigh | kAlarmWaterLow | kAlarmHumidityHigh, m.alarms);
}

void test_works_with_simulated_hal() {
    hal::ManualClock clock;
    hal::SimulatedTemperatureSensor in(clock, simulation::kAirInside, 101);
    hal::SimulatedTemperatureSensor out(clock, simulation::kAirOutside, 202);
    hal::SimulatedTemperatureSensor water(clock, simulation::kWater, 303);
    hal::SimulatedHumiditySensor rh(clock, simulation::kHumidityInside, 404);
    SensorManager manager(in, out, water, rh);
    TEST_ASSERT_TRUE(manager.begin());
    for (int i = 0; i < 100; ++i) {
        clock.set(static_cast<std::uint32_t>(i) * 60000u);
        const MeasurementData m = manager.read(clock.nowMs());
        TEST_ASSERT_TRUE(m.valid);
    }
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_all_channels_read_and_valid);
    RUN_TEST(test_sequence_increments);
    RUN_TEST(test_read_failure_marks_only_that_channel);
    RUN_TEST(test_implausible_value_is_flagged_without_alarm);
    RUN_TEST(test_threshold_alarms);
    RUN_TEST(test_works_with_simulated_hal);
    return UNITY_END();
}
