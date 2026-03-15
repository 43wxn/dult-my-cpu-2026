#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "ProgramLoader.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"
#include "test_framework.h"

using namespace loongarch;

static std::uint32_t run_and_get_exit_code(const char* path)
{
    Memory mem(16 * 1024 * 1024);
    Uart uart;
    Timer timer;
    TestDevice testDevice;
    Bus bus(mem, uart, timer, testDevice);
    CPU cpu(bus);
    ProgramLoader loader(mem);

    constexpr std::uint32_t ENTRY = 0x1000;
    constexpr std::uint64_t MAX_STEPS = 8;

    cpu.reset(ENTRY);
    testDevice.reset();

    const std::uint32_t loaded = loader.loadHexFile(path, ENTRY);
    EXPECT_TRUE(loaded > 0);

    for (std::uint64_t i = 0; i < MAX_STEPS; ++i) {
        cpu.step();
        if (testDevice.halted()) {
            return testDevice.exitCode();
        }
    }

    std::cerr << "[FAIL] program did not halt: " << path << "\n";
    return 0xFFFF'FFFFu;
}

int main() {
    EXPECT_EQ(run_and_get_exit_code("../programs/trap_pass.hex"), 0u);
    EXPECT_EQ(run_and_get_exit_code("../programs/trap_fail.hex"), 2u);
    EXPECT_EQ(run_and_get_exit_code("../programs/check_fail.hex"), 1u);

    TEST_PASS();
}