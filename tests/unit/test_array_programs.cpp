#include "SimulatorRunner.h"
#include "test_framework.h"

using namespace loongarch;

int main() {
    {
        const RunResult result =
            runHexProgram("../programs/array_pass.hex", 0x1000, 64, false);

        EXPECT_TRUE(result.loaded);
        EXPECT_TRUE(result.halted);
        EXPECT_EQ(result.exit_code, 0u);
    }

    {
        const RunResult result =
            runHexProgram("../programs/array_fail.hex", 0x1000, 64, false);

        EXPECT_TRUE(result.loaded);
        EXPECT_TRUE(result.halted);
        EXPECT_EQ(result.exit_code, 5u);
    }

    TEST_PASS();
}