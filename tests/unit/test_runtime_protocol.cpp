#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "ProgramLoader.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"
#include "test_framework.h"

using namespace loongarch;

static int run_program(const char* path,
                       std::uint32_t expected_exit_code,
                       bool expected_halt)
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

    bool halted = false;
    for (std::uint64_t i = 0; i < MAX_STEPS; ++i) {
        cpu.step();
        if (testDevice.halted()) {
            halted = true;
            break;
        }
    }

    EXPECT_EQ(halted, expected_halt);
    if (expected_halt) {
        EXPECT_EQ(testDevice.exitCode(), expected_exit_code);
    }

    return 0;
}

int main() {
    if (run_program("../programs/test_pass.hex", 0u, true) != 0) {
        return 1;
    }

    if (run_program("../programs/test_fail.hex", 1u, true) != 0) {
        return 1;
    }

    TEST_PASS();
}