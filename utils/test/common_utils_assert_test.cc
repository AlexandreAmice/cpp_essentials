#include "utils/common_utils_assert.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>
/**
 * This file is copied from Drake's
 * drake/common/test/common_utils_assert_test.cc with appropriate modifications.
 */

namespace {

GTEST_TEST(CommonUtilsAssertTest, MatchingConfigTest) {
#ifdef COMMON_UTILS_ASSERT_IS_ARMED
  EXPECT_TRUE(::common_utils::kCommonUtilsAssertIsArmed);
  EXPECT_FALSE(::common_utils::kCommonUtilsAssertIsDisarmed);
#else
  EXPECT_FALSE(::common_utils::kCommonUtilsAssertIsArmed);
  EXPECT_TRUE(::common_utils::kCommonUtilsAssertIsDisarmed);
#endif
#ifdef COMMON_UTILS_ASSERT_IS_DISARMED
  EXPECT_FALSE(::common_utils::kCommonUtilsAssertIsArmed);
  EXPECT_TRUE(::common_utils::kCommonUtilsAssertIsDisarmed);
#else
  EXPECT_TRUE(::common_utils::kCommonUtilsAssertIsArmed);
  EXPECT_FALSE(::common_utils::kCommonUtilsAssertIsDisarmed);
#endif
}

namespace {
[[maybe_unused]] void VoidFunction() {}
}  // namespace

GTEST_TEST(CommonUtilsAssertTest, AssertVoidCompilesOkay) {
  COMMON_UTILS_ASSERT_VOID(VoidFunction());
}

// Note that Drake's styleguide forbids death tests, but our only choice here
// is to use death tests because our implementation is documented to abort().

GTEST_TEST(CommonUtilsAssertDeathTest, DemandTest) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(
      { COMMON_UTILS_DEMAND(false); },
      "abort: Failure at .*common_utils_assert_test.cc:.. in TestBody..: "
      "condition 'false' failed");
}

struct BoolConvertible {
  operator bool() const { return true; }
};
GTEST_TEST(CommonUtilsAssertDeathTest, AssertSyntaxTest) {
  // These should compile.
  COMMON_UTILS_ASSERT((2 + 2) == 4);
  COMMON_UTILS_ASSERT(BoolConvertible());
}

GTEST_TEST(CommonUtilsAssertDeathTest, AssertFalseTest) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  if (::common_utils::kCommonUtilsAssertIsArmed) {
    ASSERT_DEATH(
        { COMMON_UTILS_ASSERT(2 + 2 == 5); },
        "abort: Failure at .*common_utils_assert_test.cc:.. in TestBody..: "
        "condition '2 \\+ 2 == 5' failed");
  } else {
    COMMON_UTILS_ASSERT(2 + 2 == 5);
  }
}

GTEST_TEST(CommonUtilsAssertDeathTest, AssertVoidTestArmed) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  if (::common_utils::kCommonUtilsAssertIsArmed) {
    ASSERT_DEATH({ COMMON_UTILS_ASSERT_VOID(::abort()); }, "");
  } else {
    COMMON_UTILS_ASSERT_VOID(::abort());
  }
}

}  // namespace
