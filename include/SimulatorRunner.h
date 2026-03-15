#pragma once

#include <cstdint>
#include <string>

namespace loongarch {

struct RunResult {
    bool loaded{false};
    bool halted{false};
    std::uint32_t exit_code{0};
    std::uint64_t steps{0};
};

RunResult runHexProgram(const std::string& program_path,
                        std::uint32_t entry = 0x1000,
                        std::uint64_t max_steps = 64,
                        bool trace = false);

} // namespace loongarch