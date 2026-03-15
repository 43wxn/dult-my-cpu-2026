#include "ProgramLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace loongarch
{

ProgramLoader::ProgramLoader(Memory &memory) noexcept : m_memory(memory)
{
}

std::uint32_t ProgramLoader::loadHexFile(const std::string &path, std::uint32_t load_addr)
{
    std::ifstream fin(path);
    if (!fin)
    {
        throw std::runtime_error("ProgramLoader: cannot open file: " + path);
    }

    std::string line;
    std::uint32_t count = 0;
    std::uint32_t addr = load_addr;

    while (std::getline(fin, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token.empty())
        {
            continue;
        }

        if (token[0] == '#')
        {
            continue;
        }

        std::uint32_t word = 0;
        std::stringstream ss;
        ss << std::hex << token;
        ss >> word;

        if (ss.fail())
        {
            throw std::runtime_error("ProgramLoader: invalid hex word: " + token);
        }

        m_memory.write32(static_cast<std::uint32_t>(addr), word);
        addr += 4;
        ++count;
    }

    return count;
}

std::size_t ProgramLoader::loadBinFile(const std::string &path, std::uint32_t load_addr)
{
    std::ifstream fin(path, std::ios::binary);
    if (!fin)
    {
        throw std::runtime_error("ProgramLoader: cannot open file: " + path);
    }

    std::string data((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

    if (data.empty())
    {
        throw std::runtime_error("ProgramLoader: binary file is empty: " + path);
    }

    if (data.size() % 4 != 0)
    {
        throw std::runtime_error("ProgramLoader: binary size is not a multiple of 4 bytes: " +
                                 path);
    }

    std::uint32_t addr = load_addr;
    for (std::size_t i = 0; i < data.size(); i += 4)
    {
        const auto *p = reinterpret_cast<const unsigned char *>(data.data() + i);
        std::uint32_t word =
            static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
            (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);

        m_memory.write32(static_cast<std::uint32_t>(addr), word);
        addr += 4;
    }

    return data.size();
}

std::size_t ProgramLoader::loadFileAuto(const std::string &path, std::uint32_t load_addr)
{
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".hex")
    {
        return loadHexFile(path, load_addr);
    }

    if (path.size() >= 4 && path.substr(path.size() - 4) == ".bin")
    {
        return loadBinFile(path, load_addr);
    }

    throw std::runtime_error("ProgramLoader: unsupported file extension: " + path);
}

} // namespace loongarch