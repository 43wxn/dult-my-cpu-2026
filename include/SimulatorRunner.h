#pragma once

#include "PlatformConfig.h"
#include <cstdint>
#include <string>

namespace loongarch
{

struct RunResult
{
    bool loaded{false};
    bool halted{false};
    std::uint32_t exit_code{0};
    std::uint64_t steps{0};
};

RunResult runHexProgram(const std::string &program_path,
                        std::uint32_t entry = loongarch::PlatformConfig::ENTRY,
                        std::uint64_t max_steps = loongarch::PlatformConfig::MAX_STEPS,
                        bool trace = false);

} // namespace loongarch