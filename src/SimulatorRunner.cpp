#include "SimulatorRunner.h"
#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "PlatformConfig.h"
#include "ProgramLoader.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"

#include <exception>
#include <iostream>

namespace loongarch
{

RunResult runHexProgram(const std::string &program_path, std::uint32_t entry,
                        std::uint64_t max_steps, bool trace)
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

    if (trace)
    {
        std::cout << "[SIM] program=" << program_path << "\n";
        std::cout << "[SIM] entry=0x" << std::hex << entry << std::dec
                  << " max_steps=" << max_steps << "\n";
        std::cout << "[SIM] loaded=" << loaded << " instruction(s)\n";
    }

    try
    {
        for (std::uint64_t step = 0; step < max_steps; ++step)
        {
            cpu.step();
            result.steps = step + 1;

            if (trace)
            {
                std::cout << "[SIM][step " << step + 1 << "] "
                          << "pc=0x" << std::hex << cpu.getPC()
                          << " r1=0x" << cpu.getReg(1)
                          << " r2=0x" << cpu.getReg(2)
                          << std::dec << " cycles=" << cpu.getCycleCount()
                          << " halted=" << testDevice.halted();
                if (testDevice.halted())
                {
                    std::cout << " exit=" << testDevice.exitCode();
                }
                std::cout << "\n";
            }

            if (testDevice.halted())
            {
                result.halted = true;
                result.exit_code = testDevice.exitCode();
                if (trace)
                {
                    std::cout << "[SIM] halted after " << result.steps
                              << " step(s), exit_code=" << result.exit_code << "\n";
                }
                return result;
            }
        }

        if (trace)
        {
            std::cout << "[SIM] reached max_steps without halt, steps=" << result.steps << "\n";
        }

        return result;
    }
    catch (const std::exception &)
    {
        return result;
    }
}

} // namespace loongarch
