#include "utils/test_utils/expect_no_throw.h"

#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

namespace common_utils {
namespace {

void DoesNotThrow() {}

void DoesThrow() {
  throw std::runtime_error("Big time fail");
}

void DoesThrowCrazy() {
  throw 42;
}

constexpr char failure_message[] =
    R"(Expected: Does not throw:
  DoesThrow()
Actual: Throws std::runtime_error
  Big time fail)";

constexpr char failure_message_crazy[] =
    R"(Expected: Does not throw:
  DoesThrowCrazy()
Actual: Throws type which does not inherit from std::exception)";

GTEST_TEST(ExpectNoThrowTest, ExpectNoThrow) {
  COMMON_UTILS_EXPECT_NO_THROW(DoesNotThrow());
  EXPECT_NONFATAL_FAILURE(COMMON_UTILS_EXPECT_NO_THROW(DoesThrow()),
                          failure_message);
  EXPECT_NONFATAL_FAILURE(COMMON_UTILS_EXPECT_NO_THROW(DoesThrowCrazy()),
                          failure_message_crazy);
}

GTEST_TEST(ExpectNoThrowTest, AssertNoThrow) {
  COMMON_UTILS_ASSERT_NO_THROW(DoesNotThrow());
  EXPECT_FATAL_FAILURE(COMMON_UTILS_ASSERT_NO_THROW(DoesThrow()),
                       failure_message);
  EXPECT_FATAL_FAILURE(COMMON_UTILS_ASSERT_NO_THROW(DoesThrowCrazy()),
                       failure_message_crazy);
}

}  // namespace
}  // namespace common_utils
