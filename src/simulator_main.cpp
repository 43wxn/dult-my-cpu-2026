#include "SimulatorRunner.h"

#include <cstdint>
#include <iostream>
#include <string>

using namespace loongarch;

int main(int argc, char* argv[]) {
    std::string program_path = "../programs/test_exit.hex";
    if (argc >= 2) {
        program_path = argv[1];
    }

    const RunResult result = runHexProgram(program_path, 0x1000, 64, true);

    if (!result.loaded) {
        std::cerr << "Failed to load program: " << program_path << "\n";
        return 1;
    }

    if (!result.halted) {
        std::cerr << "Program did not halt within step budget.\n";
        return 1;
    }

    std::cout << "Program halted with exit code "
              << result.exit_code << "\n";

    if (result.exit_code == 0) {
        std::cout << "[RESULT] PASS (goodtrap)\n";
        std::cout << "Simulation finished successfully.\n";
        return 0;
    } else {
        std::cout << "[RESULT] FAIL (badtrap, code="
                  << result.exit_code << ")\n";
        std::cout << "Simulation finished with failure.\n";
        return 1;
    }
}