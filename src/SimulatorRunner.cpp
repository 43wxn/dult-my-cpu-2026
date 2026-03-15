#include "SimulatorRunner.h"
#include "PlatformConfig.h"
#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "ProgramLoader.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"

#include <exception>
#include <iostream>

namespace loongarch {

RunResult runHexProgram(const std::string& program_path,
                        std::uint32_t entry,
                        std::uint64_t max_steps,
                        bool trace)
{
    Memory mem(loongarch::PlatformConfig::MEMORY_SIZE);
    Uart uart;
    Timer timer;
    TestDevice testDevice;
    Bus bus(mem, uart, timer, testDevice);
    CPU cpu(bus);
    ProgramLoader loader(mem);

    RunResult result{};

    cpu.reset(entry);
    testDevice.reset();

    const std::size_t loaded = loader.loadFileAuto(program_path, entry);
    result.loaded = (loaded > 0);

    if (trace) {
        std::cout << "Starting simulation at PC = 0x"
                  << std::hex << entry << std::dec << "\n";
        std::cout << "Loaded " << loaded
                  << " instruction(s) from " << program_path << "\n";
    }

    try {
        for (std::uint64_t step = 0; step < max_steps; ++step) {
            cpu.step();
            result.steps = step + 1;

            if (trace) {
                std::cout << "[step " << step + 1 << "] "
                          << "PC=0x" << std::hex << cpu.getPC()
                          << " r1=0x" << cpu.getReg(1)
                          << " r2=" << std::dec << cpu.getReg(2)
                          << "\n";
            }

            if (testDevice.halted()) {
                result.halted = true;
                result.exit_code = testDevice.exitCode();
                return result;
            }
        }

        return result;

    } catch (const std::exception&) {
        return result;
    }
}

} // namespace loongarch