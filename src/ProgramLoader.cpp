#include "ProgramLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace loongarch {

ProgramLoader::ProgramLoader(Memory& memory) noexcept
    : m_memory(memory) {}

std::uint32_t ProgramLoader::loadHexFile(const std::string& path, std::uint32_t load_addr)
{
    std::ifstream fin(path);
    if (!fin) {
        throw std::runtime_error("ProgramLoader: cannot open file: " + path);
    }

    std::string line;
    std::uint32_t count = 0;
    std::uint32_t addr = load_addr;

    while (std::getline(fin, line)) {
        // 去掉空行
        if (line.empty()) {
            continue;
        }

        // 去掉前导空白
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token.empty()) {
            continue;
        }

        // 支持简单注释：如果一行第一个非空字符是 '#', 跳过
        if (token[0] == '#') {
            continue;
        }

        std::uint32_t word = 0;
        std::stringstream ss;
        ss << std::hex << token;
        ss >> word;

        if (ss.fail()) {
            throw std::runtime_error("ProgramLoader: invalid hex word: " + token);
        }

        m_memory.write32(addr, word);
        addr += 4;
        ++count;
    }

    return count;
}

} // namespace loongarch