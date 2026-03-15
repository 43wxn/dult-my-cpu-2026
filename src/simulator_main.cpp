#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"

#include <cstdint>
#include <exception>
#include <iostream>

using namespace loongarch;

int main() {
    Memory mem(16 * 1024 * 1024);
    Uart uart;
    Timer timer;
    TestDevice testDevice;
    Bus bus(mem, uart, timer, testDevice);
    CPU cpu(bus);

    constexpr std::uint32_t ENTRY = 0x1000;
    constexpr std::uint64_t MAX_STEPS = 16;

    cpu.reset(ENTRY);
    testDevice.reset();

    // Program:
    //   r1 = 0x1FFFF000         (TestDevice base)
    //   r2 = 0
    //   *(uint32_t*)r1 = r2     (exit with code 0)

    mem.write32(ENTRY + 0x0, 0x143FFFE1u); // lu12i.w r1, 0x1FFFF
    mem.write32(ENTRY + 0x4, 0x02800002u); // addi.w  r2, r0, 0
    mem.write32(ENTRY + 0x8, 0x29800022u); // st.w    r2, r1, 0

    std::cout << "Starting simulation at PC = 0x"
              << std::hex << ENTRY << std::dec << "\n";

    try {
        for (std::uint64_t step = 0; step < MAX_STEPS; ++step) {
            cpu.step();

            std::cout << "[step " << step + 1 << "] "
                      << "PC=0x" << std::hex << cpu.getPC()
                      << " r1=0x" << cpu.getReg(1)
                      << " r2=" << std::dec << cpu.getReg(2)
                      << "\n";

            if (testDevice.halted()) {
                std::cout << "Program halted with exit code "
                          << testDevice.exitCode() << "\n";

                if (testDevice.exitCode() == 0) {
                    std::cout << "Simulation finished successfully.\n";
                    return 0;
                } else {
                    std::cout << "Simulation finished with failure.\n";
                    return 1;
                }
            }
        }

        std::cout << "Simulation stopped after reaching MAX_STEPS="
                  << MAX_STEPS << "\n";
        std::cout << "No halt signal observed.\n";
        return 1;

    } catch (const std::exception& e) {
        std::cerr << "Simulation exception: " << e.what() << "\n";
        cpu.dumpState();
        return 1;
    }
}