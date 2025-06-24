#include "utils/common_utils_copyable.h"

#include <utility>

#include "utils/unused.h"
#include <gtest/gtest.h>
/**
 * This file is copied from Drake's
 * drake/common/test/common_utils_copyable_test.cc with appropriate
 * modifications.
 */

namespace common_utils {
namespace {

// When a class has a user-defined destructor, the implicit generation of the
// copy constructor and copy-assignment operator is deprecated as of C++11.
//
// Under Drake's Clang configuration, relying on the deprecated operator
// produces an error under -Werror=deprecated.
//
// Therefore, if COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN failed to default those
// operations, then this unit test would no longer compile (under Clang).
class ExampleDefault {
 public:
  COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(ExampleDefault);
  ExampleDefault() = default;
  ~ExampleDefault() = default;
};

GTEST_TEST(CommonUtilsCopyableTest, DefaultCopyConstruct) {
  ExampleDefault foo;
  ExampleDefault bar(foo);
  unused(bar);
}

GTEST_TEST(CommonUtilsCopyableTest, DefaultCopyAssign) {
  ExampleDefault foo, bar;
  bar = foo;
  unused(bar);
}

GTEST_TEST(CommonUtilsCopyableTest, DefaultMoveConstruct) {
  ExampleDefault foo;
  ExampleDefault bar(std::move(foo));
  unused(bar);
}

GTEST_TEST(CommonUtilsCopyableTest, DefaultMoveAssign) {
  ExampleDefault foo, bar;
  bar = std::move(foo);
  unused(bar);
}

// Confirm that private (and by association, protected) access for this macro
// is permitted.  If not, this class would fail to compile.
class ExampleDefaultPrivate {
 private:
  COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(ExampleDefaultPrivate);
};

// When we can't use the default implementations, but are still able to provide
// the same functionality with custom implementations,
// COMMON_UTILS_DECLARE_COPY_AND_MOVE_AND_ASSIGN is used instead to provide uniform
// declarations and documentation. We're just checking here that it provides
// the right declarations.

class ExampleDeclare {
 public:
  COMMON_UTILS_DECLARE_COPY_AND_MOVE_AND_ASSIGN(ExampleDeclare);
  ExampleDeclare() = default;
  ~ExampleDeclare() = default;
};

ExampleDeclare::ExampleDeclare(const ExampleDeclare&) {}
ExampleDeclare& ExampleDeclare::operator=(const ExampleDeclare&) {
  return *this;
}
ExampleDeclare::ExampleDeclare(ExampleDeclare&&) {}
ExampleDeclare& ExampleDeclare::operator=(ExampleDeclare&&) {
  return *this;
}

GTEST_TEST(CommonUtilsCopyableTest, DeclareCopyConstruct) {
  ExampleDeclare foo;
  ExampleDeclare bar(foo);
  unused(bar);
}

GTEST_TEST(CommonUtilsCopyableTest, DeclareCopyAssign) {
  ExampleDeclare foo, bar;
  bar = foo;
  unused(bar);
}

GTEST_TEST(CommonUtilsCopyableTest, DeclareMoveConstruct) {
  ExampleDeclare foo;
  ExampleDeclare bar(std::move(foo));
  unused(bar);
}

GTEST_TEST(CommonUtilsCopyableTest, DeclareMoveAssign) {
  ExampleDeclare foo, bar;
  bar = std::move(foo);
  unused(bar);
}

// Confirm that private (and by association, protected) access for this macro
// is permitted.  If not, this class would fail to compile.
class ExampleDeclarePrivate {
 private:
  COMMON_UTILS_DECLARE_COPY_AND_MOVE_AND_ASSIGN(ExampleDeclarePrivate);
};

}  // namespace
}  // namespace common_utils
