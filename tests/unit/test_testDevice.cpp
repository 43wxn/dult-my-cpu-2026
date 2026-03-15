#include "TestDevice.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    TestDevice dev;

    EXPECT_EQ(dev.halted(), false);
    EXPECT_EQ(dev.exitCode(), 0u);

    dev.write32(TestDevice::BASE_ADDR, 7);

    EXPECT_EQ(dev.halted(), true);
    EXPECT_EQ(dev.exitCode(), 7u);
    EXPECT_EQ(dev.read32(TestDevice::BASE_ADDR), 7u);
    EXPECT_EQ(dev.read32(TestDevice::BASE_ADDR + 4), 1u);

    TEST_PASS();
}