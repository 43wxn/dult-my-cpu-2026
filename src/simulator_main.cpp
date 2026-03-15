#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "ProgramLoader.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"

using namespace loongarch;

int main(int argc, char* argv[]) {
    Memory mem(16 * 1024 * 1024);
    Uart uart;
    Timer timer;
    TestDevice testDevice;
    Bus bus(mem, uart, timer, testDevice);
    CPU cpu(bus);
    ProgramLoader loader(mem);

    constexpr std::uint32_t ENTRY = 0x1000;
    constexpr std::uint64_t MAX_STEPS = 32;

    std::string program_path = "../programs/test_exit.hex";
    if (argc >= 2) {
        program_path = argv[1];
    }

    cpu.reset(ENTRY);
    testDevice.reset();

    try {
        const std::uint32_t loaded = loader.loadHexFile(program_path, ENTRY);

        std::cout << "Starting simulation at PC = 0x"
                  << std::hex << ENTRY << std::dec << "\n";
        std::cout << "Loaded " << loaded
                  << " instruction(s) from " << program_path << "\n";

        for (std::uint64_t step = 0; step < MAX_STEPS; ++step) {
            cpu.step();

            std::cout << "[step " << (step + 1) << "] "
                      << "PC=0x" << std::hex << cpu.getPC()
                      << " r1=0x" << cpu.getReg(1)
                      << " r2=" << std::dec << cpu.getReg(2)
                      << "\n";

            if (testDevice.halted()) {
                const std::uint32_t code = testDevice.exitCode();

                std::cout << "Program halted with exit code " << code << "\n";

                if (code == 0) {
                    std::cout << "[RESULT] PASS (goodtrap)\n";
                    std::cout << "Simulation finished successfully.\n";
                    return 0;
                } else {
                    std::cout << "[RESULT] FAIL (badtrap, code=" << code << ")\n";
                    std::cout << "Simulation finished with failure.\n";
                    return 1;
                }
            }
        }

        std::cout << "Simulation stopped after reaching MAX_STEPS=" << MAX_STEPS << "\n";
        std::cout << "No halt signal observed.\n";
        return 1;

    } catch (const std::exception& e) {
        std::cerr << "Simulation exception: " << e.what() << "\n";
        cpu.dumpState();
        return 1;
    }
}