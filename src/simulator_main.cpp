#include "SimulatorRunner.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    std::string program_path = "../programs/test_exit.hex";
    if (argc >= 2)
    {
        program_path = argv[1];
    }

    const auto result = loongarch::runHexProgram(program_path, loongarch::PlatformConfig::ENTRY,
                                                 loongarch::PlatformConfig::MAX_STEPS, true);

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