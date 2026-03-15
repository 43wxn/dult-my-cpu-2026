#include "TestDevice.h"

#include <stdexcept>

namespace loongarch
{

std::uint32_t TestDevice::read32(std::uint32_t addr)
{
    const std::uint32_t offset = addr - BASE_ADDR;

    switch (offset)
    {
    case 0x0:
        return m_exit_code;
    case 0x4:
        return m_halted ? 1u : 0u;
    default:
        throw std::runtime_error("TestDevice: invalid read offset");
    }
}

void TestDevice::write32(std::uint32_t addr, std::uint32_t value)
{
    const std::uint32_t offset = addr - BASE_ADDR;

    switch (offset)
    {
    case 0x0:
        m_exit_code = value;
        m_halted = true;
        break;
    default:
        throw std::runtime_error("TestDevice: invalid write offset");
    }
}

} // namespace loongarch