#include "Bus.h"
#include "CPU.h"
#include "Memory.h"
#include "TestDevice.h"
#include "Timer.h"
#include "Uart.h"

#include <cstdint>
#include <iostream>

using namespace loongarch;

int main() {
    //初始化cpu
    Memory mem(16 * 1024 * 1024);
    Uart uart;
    Timer timer;
    TestDevice testDevice;
    Bus bus(mem, uart, timer, testDevice);
    CPU cpu(bus);

    cpu.reset(0x1000);
        // 这里先不追求复杂程序，只先验证主循环框架
    // 后面我们会用真正的程序加载器替换它

    // TODO:
    // 这里后面会写入真正的机器码程序
    // 当前先打印骨架已就绪

    std::cout << "Simulator runtime skeleton ready.\n";
    std::cout << "TestDevice MMIO base = 0x" << std::hex << TestDevice::BASE_ADDR << "\n";
    return 0;
}