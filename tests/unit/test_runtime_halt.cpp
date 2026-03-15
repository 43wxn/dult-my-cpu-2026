/*验证程序是否能写TestDevice，并退出*/
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
    constexpr std::uint64_t MAX_STEPS = 8;

    cpu.reset(ENTRY);
    testDevice.reset();

    mem.write32(ENTRY + 0x0, 0x143FFFE1u); // lu12i.w r1, 0x1FFFF
    mem.write32(ENTRY + 0x4, 0x02800002u); // addi.w  r2, r0, 0
    mem.write32(ENTRY + 0x8, 0x29800022u); // st.w    r2, r1, 0

    bool halted = false;
    for (std::uint64_t i = 0; i < MAX_STEPS; ++i) {
        cpu.step();
        if (testDevice.halted()) {
            halted = true;
            break;
        }
    }

    EXPECT_TRUE(halted);
    EXPECT_EQ(testDevice.exitCode(), 0u);
    EXPECT_EQ(cpu.getReg(1), 0x1FFFF000u);
    EXPECT_EQ(cpu.getReg(2), 0u);

    TEST_PASS();
}