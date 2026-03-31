#include "SimulatorRunner.h"
#include <exception>
#include <iostream>
#include <string>
#include <cstdint>

int main(int argc, char *argv[])
{
    std::string program_path = "../programs/test_exit.hex";
    std::uint64_t max_steps = loongarch::PlatformConfig::MAX_STEPS;
    if (argc >= 2)
    {
        program_path = argv[1];
    }
    if (argc >= 3)
    {
        try
        {
            max_steps = std::stoull(argv[2]);
        }
        catch (const std::exception &)
        {
            std::cerr << "Invalid max_steps: " << argv[2] << "\n";
            return 1;
        }
    }

    const auto result = loongarch::runHexProgram(program_path, loongarch::PlatformConfig::ENTRY,
                                                 max_steps, true);

    if (!result.loaded)
    {
        std::cerr << "Failed to load program: " << program_path << "\n";
        return 1;
    }

    if (!result.halted)
    {
        std::cerr << "Program did not halt within step budget.\n";
        return 1;
    }

    std::cout << "Program halted with exit code " << result.exit_code << "\n";
    std::cout << "Program return value: " << result.exit_code << "\n";
    std::cout << "Program total runtime: " << result.total_runtime_ns
              << " ns (" << (static_cast<double>(result.total_runtime_ns) / 1000000.0)
              << " ms)\n";

    if (result.exit_code == 0)
    {
        std::cout << "[RESULT] PASS (goodtrap)\n";
        std::cout << "Simulation finished successfully.\n";
        return 0;
    }
    else
    {
        std::cout << "[RESULT] FAIL (badtrap, code=" << result.exit_code << ")\n";
        std::cout << "Simulation finished with failure.\n";
        return 1;
    }
}
