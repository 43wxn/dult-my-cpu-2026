# dult-my-cpu-2026（LoongArch 教学 CPU 模拟器）

这是一个面向课程项目的 **LoongArch 32 位 CPU 模拟器**，目标是用尽量清晰、可扩展的代码实现：

- CPU 取指 / 译码 / 执行主循环；
- 基于总线的内存与 MMIO 设备访问；
- 运行十六进制/二进制程序镜像并判断测试程序是否“通过”；
- 用一套轻量测试框架覆盖 CPU、内存、加载器和程序级回归测试。

---

## 1. 项目实现了什么功能（CPU 与平台能力）

### 1.1 CPU 核心能力

CPU 以经典单步执行模型运行（`step()`）：

1. 检查中断；
2. 从 `PC` 取指（支持分页翻译）；
3. 译码寄存器字段和 opcode；
4. 执行指令并更新寄存器 / `PC` / CSR；
5. 维护架构约束（如 `r0` 恒为 0）。

CPU 状态包括：

- 32 个 32-bit 通用寄存器；
- 程序计数器 `PC`；
- CSR（如 `CRMD/ESTAT/EPC/ECFG`）；
- 分页基址寄存器 `PGDL`；
- 周期计数等运行状态。

同时具备基础异常机制（例如非法指令、地址异常、syscall、外部中断入口跳转等）以及分页地址转换流程。

### 1.2 已支持的典型指令族（精简实现）

当前实现覆盖了课程实验常见的整数与控制流子集，包括：

- 算术/逻辑：`add.w`、`sub.w`、`and/or/xor/nor`、`slt/sltu`、`mul.w`；
- 移位：寄存器移位 + 部分立即数移位；
- 分支跳转（用于 `if/else`、循环、数组程序等样例）；
- 访存相关（配合总线访问内存与外设）。

> 该仓库是“教学可迭代版本”，并非完整 LoongArch ISA 仿真器。

### 1.3 平台与总线（Bus + Memory + MMIO）

平台由 `Bus` 统一路由 32-bit 读写请求：

- 主内存 `Memory`：默认 16 MiB；
- `Uart`：UART MMIO；
- `Timer`：定时器 MMIO；
- `TestDevice`：测试退出设备（关键）。

`TestDevice` 的约定非常重要：

- 向 `0x1FFFF000` 写入值即触发“程序结束”；
- 写入 `0` 代表 goodtrap（测试通过）；
- 写入非 0 代表 badtrap（测试失败，值作为退出码）。

### 1.4 程序加载与运行入口

- `ProgramLoader` 支持加载 `.hex` 与 `.bin`；
- `SimulatorRunner::runHexProgram()` 负责创建整套平台、复位 CPU、加载程序、步进执行，并返回 `RunResult`（是否加载成功、是否停机、退出码、步数）；
- `src/simulator_main.cpp` 提供命令行入口，可直接跑某个测试程序。

---

## 2. CPU 测试框架说明（如何验证功能）

项目测试分成两层：

### 2.1 轻量单元测试层（`tests/unit`）

项目内置了一个极简测试头 `tests/unit/test_framework.h`，核心宏：

- `EXPECT_TRUE(...)`
- `EXPECT_EQ(...)`
- `EXPECT_THROW(...)`
- `TEST_PASS()`

每个测试文件是独立可执行程序，返回码 `0` 表示通过，非 `0` 表示失败。CMake 会把它们编译成对应二进制（如 `test_cpu_basic`、`test_memory`、`test_mmu` 等）。

覆盖点包括（示例）：

- CPU 基础执行与状态；
- 内存读写、越界行为；
- MMU/地址翻译相关行为；
- ProgramLoader 对 `.hex/.bin` 的加载；
- Runtime/trap 协议；
- 分支、load/store、数组等程序级功能。

### 2.2 程序回归层（`tests/program`）

`program_test_runner` 会读取 `tests/program/program_test_manifest.txt`，逐条运行程序并对比期望退出码：

- 样例：`test_pass.hex` / `test_fail.hex` / `trap_pass.hex` / `branch_pass.hex` / `array_pass.hex` 等；
- 判定条件：
  1) 程序成功加载；
  2) 在步数预算内停机；
  3) `TestDevice` 退出码与清单一致。

这层测试非常适合做“改完 CPU 后的回归验收”。

## 核心 
1. 在programs文件夹下放入写好的c语言测试程序
2. 将测试程序注册到test/program/c_test_manifest.txt中
3. 在项目根目录运行./toolchain/run_c_tests.sh即可从终端输出中得到测试结果
***温馨提示：可通过终端输出的step步与build_runtime/中的相应的.dump反汇编文件判断出是否有指令缺失。以及为什么改测试程序没有跑通***

---

## 3. 如何快速测试（给日常开发的最短路径）

下面是建议的“最快可复现流程”。

### 3.1 一次性构建

在仓库根目录执行：

```bash
cmake -S . -B build
cmake --build build -j
```

### 3.2 30 秒回归（推荐）

先跑程序级回归：

```bash
./build/program_test_runner 			测试cpu各项功能是否正常
```

你会看到每个程序的 `[PASS]/[FAIL]` 和最终汇总。

### 3.3 直接跑单个程序（定位问题最方便）

```bash
./build/mycpu_sim ../programs/test_pass.hex
./build/mycpu_sim ../programs/branch_fail.hex
```

- 若输出 `[RESULT] PASS (goodtrap)`，说明程序按预期成功；
- 若输出 `[RESULT] FAIL (badtrap, code=...)`，可结合退出码快速定位失败类别。

### 3.4 跑关键单元测试（最小集）

```bash
./build/test_cpu_basic
./build/test_memory
./build/test_program_loader
./build/test_mmu
```

建议顺序：

1. `test_memory`（先确认底层存储没问题）；
2. `test_program_loader`（确认镜像加载无误）；
3. `test_cpu_basic`（确认基础执行路径）；
4. `program_test_runner`（做整体回归）。

---

## 4. 目录速览

- `src/`：CPU、Bus、Memory、设备、运行器实现；
- `include/`：公共头文件与平台配置；
- `tests/unit/`：单元/协议测试；
- `tests/program/`：程序清单与程序级回归入口；
- `programs/`：用于回归的 `.hex` 程序样例；
- `runtime/`：运行时约定说明（goodtrap/badtrap）。

---

## 5. 常见开发建议

- 改指令实现后，优先跑 `program_test_runner` 看是否引入回归；
- 若某一类程序失败，再用 `mycpu_sim <case.hex>` 单例调试；
- 若新增程序样例，更新 `tests/program/program_test_manifest.txt` 把它纳入回归。
