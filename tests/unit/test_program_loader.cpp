#include "Memory.h"
#include "ProgramLoader.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    Memory mem(16 * 1024 * 1024);
    ProgramLoader loader(mem);

    constexpr std::uint32_t ENTRY = 0x1000;

    std::uint32_t loaded = loader.loadHexFile("../programs/test_exit.hex", ENTRY);

    EXPECT_EQ(loaded, 3u);
    EXPECT_EQ(mem.read32(ENTRY + 0x0), 0x143FFFE1u);
    EXPECT_EQ(mem.read32(ENTRY + 0x4), 0x02800002u);
    EXPECT_EQ(mem.read32(ENTRY + 0x8), 0x29800022u);

    TEST_PASS();
}