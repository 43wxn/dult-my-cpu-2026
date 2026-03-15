#include "Memory.h"
#include "ProgramLoader.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    Memory mem(16 * 1024 * 1024);
    ProgramLoader loader(mem);

    constexpr std::uint32_t ENTRY = 0x1000;

    std::size_t loaded = loader.loadBinFile("../build_runtime/test_main.bin", ENTRY);

    EXPECT_TRUE(loaded > 0);

    // 至少确认前几个字被成功写入内存
    // 这里只检查“非零”更稳，因为具体机器码由编译器生成
    EXPECT_TRUE(mem.read32(static_cast<std::uint32_t>(ENTRY + 0x0)) != 0u);
    EXPECT_TRUE(mem.read32(static_cast<std::uint32_t>(ENTRY + 0x4)) != 0u);

    TEST_PASS();
}