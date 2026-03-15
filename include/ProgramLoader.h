#pragma once

#include "Memory.h"

#include <cstdint>
#include <string>

namespace loongarch {

class ProgramLoader {
public:
    explicit ProgramLoader(Memory& memory) noexcept;

    // Load a text hex program file into memory starting at load_addr.
    // Returns number of 32-bit instructions loaded.
    std::uint32_t loadHexFile(const std::string& path, std::uint32_t load_addr);

private:
    Memory& m_memory;
};

} // namespace loongarch