#include <unity.h>

void setUp() {}
void tearDown() {}

#include <cmath>

#include "hal/simulated_sensors.h"
#include "simulation_profiles.h"

using hal::Fault;
using hal::ManualClock;
using hal::SimulatedTemperatureSensor;

void test_same_seed_and_time_give_same_value() {
    ManualClock clock;
    clock.set(123456);
    SimulatedTemperatureSensor a(clock, simulation::kAirInside, 7);
    SimulatedTemperatureSensor b(clock, simulation::kAirInside, 7);
    float va = 0, vb = 0;
    TEST_ASSERT_TRUE(a.readCelsius(va));
    TEST_ASSERT_TRUE(b.readCelsius(vb));
    TEST_ASSERT_EQUAL_FLOAT(va, vb);
}

void test_values_stay_within_profile_over_a_day() {
    ManualClock clock;
    SimulatedTemperatureSensor sensor(clock, simulation::kAirInside, 1);
    const float lo = simulation::kAirInside.base - simulation::kAirInside.amplitude - simulation::kAirInside.noise;
    const float hi = simulation::kAirInside.base + simulation::kAirInside.amplitude + simulation::kAirInside.noise;
    for (std::uint32_t s = 0; s < 86400; s += 300) {
        clock.set(s * 1000u);
        float v = 0;
        TEST_ASSERT_TRUE(sensor.readCelsius(v));
        TEST_ASSERT_TRUE(v >= lo - 0.001f && v <= hi + 0.001f);
    }
}

void test_time_scale_compresses_the_cycle() {
    ManualClock clock;
    SimulatedTemperatureSensor realtime(clock, simulation::kAirInside, 1, 1);
    SimulatedTemperatureSensor fast(clock, simulation::kAirInside, 1, 120);
    // 1/4 of a simulated day at 120x is 3 real minutes: the peak of the sine.
    clock.set(180u * 1000u);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, simulation::kAirInside.base + simulation::kAirInside.amplitude,
                             fast.channel().idealValue());
    TEST_ASSERT_TRUE(realtime.channel().idealValue() < fast.channel().idealValue());
}

void test_injected_read_error_then_recovers() {
    ManualClock clock;
    SimulatedTemperatureSensor sensor(clock, simulation::kWater, 3);
    sensor.channel().injectFault(Fault::ReadError, 2);
    float v = 0;
    TEST_ASSERT_FALSE(sensor.readCelsius(v));
    TEST_ASSERT_FALSE(sensor.readCelsius(v));
    TEST_ASSERT_TRUE(sensor.readCelsius(v));
    TEST_ASSERT_TRUE(std::isfinite(v));
}

void test_injected_nan_and_spike() {
    ManualClock clock;
    SimulatedTemperatureSensor sensor(clock, simulation::kWater, 3);
    float v = 0;
    sensor.channel().injectFault(Fault::NotANumber, 1);
    TEST_ASSERT_TRUE(sensor.readCelsius(v));
    TEST_ASSERT_TRUE(std::isnan(v));
    sensor.channel().injectFault(Fault::Spike, 1);
    TEST_ASSERT_TRUE(sensor.readCelsius(v));
    TEST_ASSERT_TRUE(v > 100.0f);
}

void test_offset_shifts_signal() {
    ManualClock clock;
    SimulatedTemperatureSensor sensor(clock, simulation::kAirInside, 5);
    const float before = sensor.channel().idealValue();
    sensor.channel().setOffset(7.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, before + 7.0f, sensor.channel().idealValue());
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_same_seed_and_time_give_same_value);
    RUN_TEST(test_values_stay_within_profile_over_a_day);
    RUN_TEST(test_time_scale_compresses_the_cycle);
    RUN_TEST(test_injected_read_error_then_recovers);
    RUN_TEST(test_injected_nan_and_spike);
    RUN_TEST(test_offset_shifts_signal);
    return UNITY_END();
}
