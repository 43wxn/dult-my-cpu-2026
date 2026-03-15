#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    Memory mem(16 * 1024 * 1024);
    Uart uart;
    Timer timer;
    TestDevice testDevice;
    Bus bus(mem, uart, timer, testDevice);
    CPU cpu(bus);

    constexpr std::uint32_t ENTRY = 0x1000;

    cpu.reset(ENTRY);
    testDevice.reset();

    // 已知可运行指令：ADDI.W r1, r0, 42
    mem.write32(ENTRY + 0, 0x0280A801);

    cpu.step();

    EXPECT_EQ(cpu.getReg(1), 42u);
    EXPECT_EQ(cpu.getPC(), ENTRY + 4);
    EXPECT_EQ(cpu.getCycleCount(), 1u);
    EXPECT_EQ(testDevice.halted(), false);

    TEST_PASS();
}