#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "Timer.h"
#include "Uart.h"
#include "test_framework.h"

int main() {
    loongarch::Memory mem(16 * 1024 * 1024);
    loongarch::Uart uart;
    loongarch::Timer timer;
    loongarch::Bus bus(mem, uart, timer);
    loongarch::CPU cpu(bus);

    mem.write32(0x1000, 0x0280A801);

    cpu.setPC(0x1000);
    cpu.step();

    EXPECT_EQ(cpu.registers()[1], 42u);
    EXPECT_EQ(cpu.getPC(), 0x1004u);

    TEST_PASS();
}