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
#include <chrono>
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
        const auto run_start = std::chrono::steady_clock::now();
        for (std::uint64_t step = 0; step < max_steps; ++step)
        {
            const auto step_start = std::chrono::steady_clock::now();
            cpu.step();
            const auto step_end = std::chrono::steady_clock::now();
            result.steps = step + 1;
            const auto step_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(step_end - step_start).count();
            result.total_runtime_ns += static_cast<std::uint64_t>(step_ns);

            if (trace)
            {
                std::cout << "[SIM][step " << step + 1 << "] "
                          << "pc=0x" << std::hex << cpu.getPC()
                          << " r1=0x" << cpu.getReg(1)
                          << " r2=0x" << cpu.getReg(2)
                          << std::dec << " cycles=" << cpu.getCycleCount()
                          << " halted=" << testDevice.halted()
                          << " step_time_ns=" << step_ns;
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
                    std::cout << "[SIM] total_runtime_ns=" << result.total_runtime_ns
                              << " total_runtime_ms=" << (static_cast<double>(result.total_runtime_ns) / 1'000'000.0)
                              << "\n";
                }
                return result;
            }
        }

        if (trace)
        {
            const auto run_end = std::chrono::steady_clock::now();
            const auto wall_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(run_end - run_start).count();
            std::cout << "[SIM] reached max_steps without halt, steps=" << result.steps << "\n";
            std::cout << "[SIM] accumulated_step_runtime_ns=" << result.total_runtime_ns
                      << " wall_runtime_ns=" << wall_ns << "\n";
        }

        return result;
    }
    catch (const std::exception &)
    {
        return result;
    }
}

} // namespace loongarch
