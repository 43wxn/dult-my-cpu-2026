#pragma once

#include "Device.h"
#include <cstdint>

namespace loongarch
{

class TestDevice : public Device
{
  public:
    static constexpr std::uint32_t BASE_ADDR = 0x1FFFF000u;
    static constexpr std::uint32_t SIZE = 0x1000u;

    [[nodiscard]] std::uint32_t read32(std::uint32_t addr) override;
    void write32(std::uint32_t addr, std::uint32_t value) override;

    [[nodiscard]] bool halted() const noexcept
    {
        return m_halted;
    }
    [[nodiscard]] std::uint32_t exitCode() const noexcept
    {
        return m_exit_code;
    }

    void reset() noexcept
    {
        m_halted = false;
        m_exit_code = 0;
    }

  private:
    bool m_halted{false};
    std::uint32_t m_exit_code{0};
};

} // namespace loongarch