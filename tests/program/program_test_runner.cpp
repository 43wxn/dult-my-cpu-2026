#include "SimulatorRunner.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace loongarch;

struct ProgramCase {
    std::string name;
    std::string path;
    std::uint32_t expected_exit_code;
};

static std::vector<ProgramCase> load_manifest(const std::string& manifest_path) {
    std::vector<ProgramCase> cases;
    std::ifstream fin(manifest_path);
    if (!fin) {
        throw std::runtime_error("Cannot open manifest: " + manifest_path);
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }
        if (line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        ProgramCase tc;
        if (!(iss >> tc.name >> tc.path >> tc.expected_exit_code)) {
            throw std::runtime_error("Invalid manifest line: " + line);
        }
        cases.push_back(tc);
    }

    return cases;
}

int main() {
    try {
        const auto cases = load_manifest("../tests/program/program_test_manifest.txt");

        std::size_t passed = 0;
        std::size_t failed = 0;

        std::cout << "=== Program Test Runner ===\n";

        for (const auto& tc : cases) {
            const RunResult result = runHexProgram(tc.path, 0x1000, 64, false);

            bool ok = result.loaded &&
                      result.halted &&
                      result.exit_code == tc.expected_exit_code;

            if (ok) {
                ++passed;
                std::cout << "[PASS] "
                          << std::left << std::setw(14) << tc.name
                          << " exit=" << result.exit_code << "\n";
            } else {
                ++failed;
                std::cout << "[FAIL] "
                          << std::left << std::setw(14) << tc.name
                          << " expected=" << tc.expected_exit_code
                          << " got=" << result.exit_code
                          << " loaded=" << result.loaded
                          << " halted=" << result.halted
                          << "\n";
            }
        }

        std::cout << "\nSummary: "
                  << passed << " passed, "
                  << failed << " failed, "
                  << cases.size() << " total\n";

        return (failed == 0) ? 0 : 1;

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }
}