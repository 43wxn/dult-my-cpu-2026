#pragma once

#include <cstdint>
#include <cstddef>

namespace loongarch {

struct PlatformConfig {
    static constexpr std::uint32_t ENTRY       = 0x1000;
    static constexpr std::uint32_t DATA_BASE   = 0x2000;
    static constexpr std::uint32_t STACK_TOP   = 0xF000;
    static constexpr std::uint32_t TEST_MMIO   = 0x1FFFF000;
    static constexpr std::size_t   MEMORY_SIZE = 16 * 1024 * 1024;
    static constexpr std::uint64_t MAX_STEPS   = 64;
};

} // namespace loongarch