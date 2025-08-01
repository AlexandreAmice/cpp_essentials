#include "utils/common_utils_throw.h"

#include <stdexcept>

#include <gtest/gtest.h>
/**
 * This file is copied from Drake's drake/common/test/drake_throw_test.cc with
 * appropriate modifications.
 */
namespace {

GTEST_TEST(CommonUtilsThrowTest, UnlessTest) {
  EXPECT_NO_THROW(COMMON_UTILS_THROW_UNLESS(true));
  EXPECT_THROW(COMMON_UTILS_THROW_UNLESS(false), std::runtime_error);
}

GTEST_TEST(CommonUtilsThrowTest, IfTest) {
  EXPECT_NO_THROW(COMMON_UTILS_THROW_IF(false));
  EXPECT_THROW(COMMON_UTILS_THROW_IF(true), std::runtime_error);
}

}  // namespace
