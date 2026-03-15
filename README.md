# dut-LoongArch-VMS

这是一个极简的 LoongArch 架构模拟器示例（CPU + 物理内存 + 简单 MMIO 设备），
用于演示基本的取指/执行流程以及分页（MMU）地址转换与页错误处理的测试用例。

## 目录结构

- `src/` - 源代码实现
	- `main.cpp` - 测试程序（包含 `test_mmu()`）
	- `CPU.cpp` - CPU 核心实现（Fetch/Decode/Execute、异常、分页翻译）
	- `Memory.cpp` - 物理内存设备（按字节存储、支持对齐的 32-bit 访问）
	- `Bus.cpp` - 简单总线，负责将访问路由到主内存或 MMIO 设备
	- `Timer.cpp` - 简单可编程定时器 MMIO 设备（支持中断触发）
	- `Uart.cpp` - 简单 UART（写低 8 位字节到 stdout）

- `include/` - 头文件
	- `CPU.h`, `Memory.h`, `Bus.h`, `Timer.h`, `Uart.h`, `Device.h`, `decoder.h`

## 项目目标

- 演示最小可用的 CPU 模型与外设交互。
- 实现并测试分页（一级页目录 + 页表）到物理地址的转换。
- 提供一个简单的测试用例：把虚拟地址映射到物理地址，写入一条立即数指令（ADDI.W），
	然后在启用分页后执行取指并验证寄存器结果；还测试对未映射虚拟地址的页故障处理。

## 依赖与环境

- 需要一个支持 C++17 的编译器（例如 `g++` 或 `clang++`）。
- 在常见 Linux 环境下用以下命令即可构建和运行（无需额外第三方库）。

## 构建（单命令）

在仓库根目录下运行：

```bash
g++ -std=c++17 -O2 src/*.cpp -I include -o loong-sim
```

说明：如果你更喜欢使用 `cmake`，可以手动创建简单的 CMakeLists.txt，然后使用常规的 CMake 构建流程。

## 运行测试

构建完成后，运行：

```bash
./loong-sim
```

程序将执行 `test_mmu()`：

- 在物理内存中创建页目录和页表，把虚拟地址 `0x12345000` 映射到物理地址 `0x0000A000`。
- 在物理地址 `0x0000A000` 写入 `ADDI.W r1, r0, 42` 指令。
- 启用分页（设置 CRMD 的 PG 位）并将 CPU 的 PC 设为虚拟地址 `0x12345000`，执行一次 `cpu.step()`。
	- 期望 `r1 == 42`（表示取指、地址转换与指令执行都正确）。
- 然后把 PC 设为未映射的虚拟地址 `0x20000000` 并执行 `cpu.step()`，
	- 期望 CPU 因页故障跳转到异常入口 `0x1C000200`（由 EXC_ADDR_ERROR 生成）。

示例预期输出（关键片段）：

```
=== LoongArch MMU / Paging Test ===
[MMU] Creating Page Directory at physical 0x00100000
[MMU] Creating Page Table at physical 0x00200000
[MMU] Mapping Virtual 0x12345000 -> Physical 0x0000A000

[MMU] Executing mapped instruction at VADDR 0x12345000
[MMU] SUCCESS: Fetched and executed instruction correctly. r1 = 42
[MMU] PC is now: 0x12345004

[MMU] Testing Page Fault by fetching from unmapped VADDR 0x20000000
[MMU] After page fault, PC is: 0x1C000200
[MMU] SUCCESS: Page fault correctly routed to exception handler.
```

（你的实际输出可能包含更多调试/错误信息，具体依赖于实现完整性。）

## 关键实现要点

- `Memory`：默认大小为 16 MiB（见 `include/Memory.h`），支持对齐的 32-bit 读写并在越界或错位访问时抛出 `std::runtime_error`。
- `Bus`：将地址路由到内存或 MMIO 设备；MMIO 包括 `Uart`（默认基址 `0x1FE001E0`）和 `Timer`（基址 `0x1FE00100`）。
- `CPU`：实现了简化的 Fetch/Decode/Execute；包含基本异常处理、CSR（EPC/ESTAT/CRMD/ECFG）以及分页翻译逻辑（`translate_address()`）。
- `decoder.h`：提供位域提取与常用解码器函数及 opcode 常量，`main.cpp` 中用它来生成测试指令。

## 如何理解 MMU 测试（更详细）

1. 在物理地址 `PD_BASE = 0x0010_0000` 写入 PDE，指向 `PT_BASE = 0x0020_0000`（并置位 Valid 位）。
2. 在 `PT_BASE + index*4` 写入 PTE，PTE 的物理页基址为 `0x0000_A000`（置位 Valid 位）。
3. 在物理内存 `0x0000_A000` 写入一条 32-bit 指令（由 `encode_2ri12(OPC2_ADDI_W, 1,0,42)` 生成）。
4. CPU 启用分页并将 `PGDL` 指向 `PD_BASE`，然后以虚拟地址 `0x12345000` 作为 PC 取指。
	 - CPU 的 `translate_address()` 会读取 PDE/PTE（通过 `Bus::read32()`）并计算物理地址，然后从物理内存取指。
5. 如果页表项无效或不存在，CPU 会产生地址异常（页故障），并跳转到异常入口。

## 扩展建议

- 完善指令集支持（`CPU.cpp` 中有很多 opcode 分支仍为骨架或注释省略）。
- 增加权限检查（读/写/执行位）与更完整的异常分类。
- 增加更完整的测试套件（多个映射、跨页访问、写保护、TLB 缓存模拟等）。
- 添加 `CMakeLists.txt` 以支持更方便的构建和单元测试集成（例如使用 `CTest`）。
---


