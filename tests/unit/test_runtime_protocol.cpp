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
    TEST_INFO("[RUNTIME_PROTOCOL] 准备运行程序: " << path);
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
    TEST_INFO("[RUNTIME_PROTOCOL] 加载指令数: " << loaded);
    EXPECT_TRUE(loaded > 0);

    bool halted = false;
    for (std::uint64_t i = 0; i < MAX_STEPS; ++i) {
        TEST_INFO("[RUNTIME_PROTOCOL] step=" << i + 1 << " pc(before)=0x"
                  << std::hex << cpu.getPC() << std::dec
                  << " r1=" << cpu.getReg(1) << " r2=" << cpu.getReg(2));
        cpu.step();
        TEST_INFO("[RUNTIME_PROTOCOL] step=" << i + 1 << " pc(after)=0x"
                  << std::hex << cpu.getPC() << std::dec
                  << " cycle=" << cpu.getCycleCount()
                  << " halted=" << testDevice.halted());
        if (testDevice.halted()) {
            halted = true;
            TEST_INFO("[RUNTIME_PROTOCOL] 程序触发退出，exitCode=" << testDevice.exitCode());
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
