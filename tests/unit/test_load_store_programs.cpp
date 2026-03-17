#include "SimulatorRunner.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    TEST_INFO("[LOAD_STORE] 开始执行 load/store 程序组测试");
    {
        const RunResult result =
            runHexProgram("../programs/load_store_pass.hex", 0x1000, 32, true);

        TEST_INFO("[LOAD_STORE] pass 程序结果: loaded=" << result.loaded
                  << " halted=" << result.halted << " exit=" << result.exit_code
                  << " steps=" << result.steps);

        EXPECT_TRUE(result.loaded);
        EXPECT_TRUE(result.halted);
        EXPECT_EQ(result.exit_code, 0u);
    }

    {
        const RunResult result =
            runHexProgram("../programs/load_store_fail.hex", 0x1000, 32, true);

        TEST_INFO("[LOAD_STORE] fail 程序结果: loaded=" << result.loaded
                  << " halted=" << result.halted << " exit=" << result.exit_code
                  << " steps=" << result.steps);

        EXPECT_TRUE(result.loaded);
        EXPECT_TRUE(result.halted);
        EXPECT_EQ(result.exit_code, 4u);
    }

    TEST_PASS();
}
