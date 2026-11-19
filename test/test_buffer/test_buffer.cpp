#include <unity.h>

void setUp() {}
void tearDown() {}

#include "payload_buffer.h"

void test_fifo_order() {
    PayloadBuffer<3, 16> buffer;
    buffer.push("a");
    buffer.push("b");
    TEST_ASSERT_EQUAL_STRING("a", buffer.front());
    buffer.pop();
    TEST_ASSERT_EQUAL_STRING("b", buffer.front());
    buffer.pop();
    TEST_ASSERT_TRUE(buffer.empty());
    TEST_ASSERT_NULL(buffer.front());
}

void test_overflow_drops_oldest_and_counts() {
    PayloadBuffer<3, 16> buffer;
    TEST_ASSERT_TRUE(buffer.push("1"));
    TEST_ASSERT_TRUE(buffer.push("2"));
    TEST_ASSERT_TRUE(buffer.push("3"));
    TEST_ASSERT_FALSE(buffer.push("4"));
    TEST_ASSERT_EQUAL_UINT32(1, buffer.dropped());
    TEST_ASSERT_EQUAL_UINT32(3, buffer.size());
    TEST_ASSERT_EQUAL_STRING("2", buffer.front());
}

void test_long_payload_is_truncated_safely() {
    PayloadBuffer<1, 4> buffer;
    buffer.push("abcdef");
    TEST_ASSERT_EQUAL_STRING("abc", buffer.front());
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_fifo_order);
    RUN_TEST(test_overflow_drops_oldest_and_counts);
    RUN_TEST(test_long_payload_is_truncated_safely);
    return UNITY_END();
}
