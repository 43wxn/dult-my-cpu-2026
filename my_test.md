# CPU 完整测试说明（含 C 源程序逐步耗时与结果值输出）

本文档说明如何在本项目中进行完整 CPU 测试，并验证以下新增输出能力：

1. 执行 C 程序时，终端会输出每一步指令执行耗时；
2. 程序结束时会输出总运行耗时；
3. 程序结束时会直接输出 C 程序返回值（例如 `3`），而不是只能看 PASS/FAIL。

---

## 1. 环境准备

在仓库根目录执行（示例路径：`/workspace/dult-my-cpu-2026`）：

```bash
cmake -S . -B build
cmake --build build -j
```

> 若要编译 C 测试程序，请确保已安装 LoongArch32 交叉编译工具链。默认脚本使用：
> `$HOME/loongarch32-toolchain/install/bin/loongarch32-unknown-elf-gcc`。

---

## 2. 运行 CPU 回归（程序级）

运行项目已有程序回归：

```bash
./build/program_test_runner
```

这一步用于确认原有 hex 程序回归行为正常。

---

## 3. 运行 C 程序测试（推荐主流程）

执行：

```bash
./toolchain/run_c_tests.sh
```

脚本会按 `tests/program/c_test_manifest.txt` 中配置逐个：

1. 编译 `programs/*.c`；
2. 生成 `build_runtime/*.bin`；
3. 调用 `build/mycpu_sim` 运行；
4. 打印程序返回值与对比结果。

你会在输出中看到类似内容：

- 每步耗时（来自模拟器）：`[SIM][step N] ... step_time_ns=...`
- 总耗时（来自模拟器）：`[SIM] total_runtime_ns=... total_runtime_ms=...`
- 程序返回值（新增）：`Program return value: ...`
- 脚本汇总返回值（新增）：`[RESULT] <case_name> return_value=...`

---

## 4. 单独运行某个 C 程序（定位问题最方便）

### 4.1 编译单个 C 程序

```bash
./toolchain/build_c_program.sh programs/test_add.c
```

会生成：

- `build_runtime/test_add.elf`
- `build_runtime/test_add.bin`
- `build_runtime/test_add.dump`

### 4.2 运行该程序

```bash
./build/mycpu_sim build_runtime/test_add.bin
```

关键观察点：

- 每一步执行时间：`step_time_ns`；
- 总执行时间：`Program total runtime: ...`；
- 程序结果值：`Program return value: ...`。

> 例如若你的 C 程序 `main` 返回 `3`，终端会直接显示 `Program return value: 3`。

---

## 5. 新增 C 用例的完整流程

1. 将新 C 文件放入 `programs/`，如 `programs/my_case.c`；
2. 在 `tests/program/c_test_manifest.txt` 添加一行：

```txt
my_case   programs/my_case.c   <expected_exit_code>
```

3. 执行：

```bash
./toolchain/run_c_tests.sh
```

查看：

- `[RESULT] my_case return_value=...`（程序实际返回值）；
- `[PASS]/[FAIL]`（与清单期望值是否一致）。

---

## 6. 常见问题

### Q1：为什么 return 非 0 会显示 FAIL？

这是项目原有 trap 约定：`0` 表示 goodtrap，非 0 表示 badtrap（退出码即返回值）。

### Q2：我只想看程序返回值，不关心 PASS/FAIL。

直接关注两行：

- `Program return value: ...`
- `[RESULT] <case_name> return_value=...`

即可。

### Q3：时间单位怎么看？

- 单步：`step_time_ns`（纳秒）；
- 总时间：同时输出 `ns` 与 `ms`。
