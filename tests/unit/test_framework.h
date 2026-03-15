/*测试框架*/
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>

#define EXPECT_TRUE(cond)                                                       \
    do {                                                                        \
        if (!(cond)) {                                                          \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__               \
                      << " EXPECT_TRUE(" #cond ") failed\n";                    \
            return 1;                                                           \
        }                                                                       \
    } while (0)

#define EXPECT_EQ(actual, expected)                                             \
    do {                                                                        \
        auto _a = (actual);                                                     \
        auto _e = (expected);                                                   \
        if (!(_a == _e)) {                                                      \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__               \
                      << " EXPECT_EQ failed\n"                                  \
                      << "  actual   = " << _a << "\n"                          \
                      << "  expected = " << _e << "\n";                         \
            return 1;                                                           \
        }                                                                       \
    } while (0)

#define EXPECT_THROW(stmt)                                                      \
    do {                                                                        \
        bool _thrown = false;                                                   \
        try {                                                                   \
            stmt;                                                               \
        } catch (...) {                                                         \
            _thrown = true;                                                     \
        }                                                                       \
        if (!_thrown) {                                                         \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__               \
                      << " EXPECT_THROW(" #stmt ") failed\n";                   \
            return 1;                                                           \
        }                                                                       \
    } while (0)

#define TEST_PASS()                                                             \
    do {                                                                        \
        std::cout << "[PASS] " << __FILE__ << "\n";                             \
        return 0;                                                               \
    } while (0)

#endif