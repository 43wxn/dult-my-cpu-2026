#include "SimulatorRunner.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    TEST_INFO("[ARRAY] 开始数组相关程序测试");
    {
        const RunResult result =
            runHexProgram("../programs/array_pass.hex", 0x1000, 64, true);

        TEST_INFO("[ARRAY] pass 程序结果: loaded=" << result.loaded
                  << " halted=" << result.halted << " exit=" << result.exit_code
                  << " steps=" << result.steps);

        EXPECT_TRUE(result.loaded);
        EXPECT_TRUE(result.halted);
        EXPECT_EQ(result.exit_code, 0u);
    }

    {
        const RunResult result =
            runHexProgram("../programs/array_fail.hex", 0x1000, 64, true);

        TEST_INFO("[ARRAY] fail 程序结果: loaded=" << result.loaded
                  << " halted=" << result.halted << " exit=" << result.exit_code
                  << " steps=" << result.steps);

        EXPECT_TRUE(result.loaded);
        EXPECT_TRUE(result.halted);
        EXPECT_EQ(result.exit_code, 5u);
    }

    TEST_PASS();
}
