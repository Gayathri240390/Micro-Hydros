#include <unity.h>

void setUp() {}
void tearDown() {}

#include <cstring>
#include <limits>

#include "payload.h"

namespace {
MeasurementData sample() {
    MeasurementData m{};
    m.sequence = 42;
    m.timestampMs = 210000;
    m.airInsideC = 23.414f;
    m.airOutsideC = 21.0f;
    m.waterC = 20.55f;
    m.humidityInsidePct = 61.8f;
    m.valid = true;
    return m;
}
}  // namespace

void test_valid_measurement_json() {
    char buf[payload::kMaxPayloadSize];
    const std::size_t n = payload::toJson(sample(), "mh-01", buf, sizeof(buf));
    TEST_ASSERT_TRUE(n > 0);
    TEST_ASSERT_EQUAL_STRING(
        "{\"schema\":1,\"device\":\"mh-01\",\"seq\":42,\"uptime_ms\":210000,\"time_scale\":1,"
        "\"air_inside_c\":23.41,\"air_outside_c\":21.00,\"water_c\":20.55,"
        "\"humidity_inside_pct\":61.80,\"valid\":true,\"faults\":[],\"alarms\":[]}",
        buf);
    TEST_ASSERT_EQUAL_UINT32(std::strlen(buf), n);
}

void test_faulty_channels_become_null_and_listed() {
    MeasurementData m = sample();
    m.waterC = std::numeric_limits<float>::quiet_NaN();
    m.readFailures = kWater;
    m.implausible = kHumidityInside;
    m.valid = false;
    m.alarms = kAlarmAirInsideHigh;
    char buf[payload::kMaxPayloadSize];
    TEST_ASSERT_TRUE(payload::toJson(m, "mh-01", buf, sizeof(buf)) > 0);
    TEST_ASSERT_NOT_NULL(std::strstr(buf, "\"water_c\":null"));
    TEST_ASSERT_NOT_NULL(std::strstr(buf, "\"humidity_inside_pct\":null"));
    TEST_ASSERT_NOT_NULL(std::strstr(buf, "\"faults\":[\"water\",\"humidity_inside\"]"));
    TEST_ASSERT_NOT_NULL(std::strstr(buf, "\"alarms\":[\"air_inside_high\"]"));
    TEST_ASSERT_NULL(std::strstr(buf, "nan"));
}

void test_worst_case_fits_max_size() {
    MeasurementData m = sample();
    m.sequence = 4294967295u;
    m.timestampMs = 4294967295u;
    m.alarms = 0x3F;
    m.readFailures = kAllChannels;
    char buf[payload::kMaxPayloadSize];
    TEST_ASSERT_TRUE(payload::toJson(m, "mh-0123456789abcdef", buf, sizeof(buf), 4294967295ul) > 0);
}

void test_too_small_buffer_returns_zero() {
    char buf[32];
    TEST_ASSERT_EQUAL_UINT32(0, payload::toJson(sample(), "mh-01", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("", buf);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_valid_measurement_json);
    RUN_TEST(test_faulty_channels_become_null_and_listed);
    RUN_TEST(test_worst_case_fits_max_size);
    RUN_TEST(test_too_small_buffer_returns_zero);
    return UNITY_END();
}
