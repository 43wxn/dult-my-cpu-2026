#include "Memory.h"
#include "test_framework.h"

int main() {
    loongarch::Memory mem(1024);

    mem.write32(0, 0x12345678);
    EXPECT_EQ(mem.read32(0), 0x12345678u);

    mem.write32(4, 0xAABBCCDD);
    EXPECT_EQ(mem.read32(4), 0xAABBCCDDu);

    EXPECT_THROW((void)mem.read32(1024));
    EXPECT_THROW(mem.write32(1024, 1));

    EXPECT_THROW((void)mem.read32(2));
    EXPECT_THROW(mem.write32(2, 0x55));

    TEST_PASS();
}