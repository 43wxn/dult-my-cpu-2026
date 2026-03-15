#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
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

    cpu.reset(0x1000);

    EXPECT_EQ(cpu.getPC(), 0x1000u);
    EXPECT_EQ(cpu.getReg(0), 0u);
    EXPECT_EQ(cpu.getReg(1), 0u);
    EXPECT_EQ(cpu.getCycleCount(), 0u);

    cpu.setReg(1, 123);
    EXPECT_EQ(cpu.getReg(1), 123u);

    cpu.setReg(0, 999);
    EXPECT_EQ(cpu.getReg(0), 0u);  // r0 must remain zero

    TEST_PASS();
}