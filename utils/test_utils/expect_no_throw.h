#pragma once

#include <exception>

#include "utils/nice_type_name.h"
#include <gtest/gtest.h>
/**
 * Copied from Drake's drake/common/test/expect_no_throw.h with appropriate
 * modifications.
 */
// Private helper macro to ensure error messages are the same for EXPECT and
// ASSERT variants of this macro.
// TODO(eric.cousineau): Add support for ostream additions, e.g.
// `COMMON_UTILS_EXPECT_NO_THROW(stuff) << "Message"`, like
// `GTEST_TEST_NO_THROW_` does with labels. (Though TBH, `SCOPED_TRACE(...)` is
// better.)
#define _COMMON_UTILS_TEST_NO_THROW(statement, fail_macro)                     \
  try {                                                                        \
    statement;                                                                 \
  } catch (const std::exception& e) {                                          \
    fail_macro() << "Expected: Does not throw:\n  " << #statement << std::endl \
                 << "Actual: Throws " << ::common_utils::NiceTypeName::Get(e)  \
                 << std::endl                                                  \
                 << "  " << e.what();                                          \
  } catch (...) {                                                              \
    fail_macro()                                                               \
        << "Expected: Does not throw:\n  " << #statement << std::endl          \
        << "Actual: Throws type which does not inherit from std::exception";   \
  }

/**
Unittest helper to explicitly indicate that a statement should not throw an
exception. This should be used in place of EXPECT_NO_THROW because it will have
useful information when a failure indeed happens.
*/
#define COMMON_UTILS_EXPECT_NO_THROW(statement) \
  _COMMON_UTILS_TEST_NO_THROW(statement, ADD_FAILURE)

/**
Same as COMMON_UTILS_EXPECT_NO_THROW, but halts the execution of the given
test case on failure.
*/
#define COMMON_UTILS_ASSERT_NO_THROW(statement) \
  _COMMON_UTILS_TEST_NO_THROW(statement, GTEST_FAIL)
