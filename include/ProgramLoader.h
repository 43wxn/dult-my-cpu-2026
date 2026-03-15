#pragma once

#include "Memory.h"
#include <cstdint>
#include <string>

namespace loongarch
{

class ProgramLoader
{
  public:
    explicit ProgramLoader(Memory &memory) noexcept;

    std::uint32_t loadHexFile(const std::string &path, std::uint32_t load_addr);
    std::size_t loadBinFile(const std::string &path, std::uint32_t load_addr);
    std::size_t loadFileAuto(const std::string &path, std::uint32_t load_addr);

  private:
    Memory &m_memory;
};

} // namespace loongarch