#pragma once

#include <type_traits>

/**
 * This file is copied from Drake's drake/common/drake_assert.h with appropriate
 * modifications.
 *
 */

#ifdef COMMON_UTILS_DOXYGEN_CXX
/// @p COMMON_UTILS_ASSERT(condition) is similar to the built-in @p
/// assert(condition) from the C++ system header `<cassert>`.  Unless
/// CommonUtils' assertions are disarmed by the pre-processor definitions listed
/// below, @p COMMON_UTILS_ASSERT will evaluate @p condition and iff the value
/// is false will trigger an assertion failure with a message showing at least
/// the condition text, function name, file, and line.
///
/// By default, assertion failures will :abort() the program.  However, when
/// using the python bindings, assertion failures will instead throw a
/// C++ exception that causes a python SystemExit exception.
///
/// Assertions are enabled or disabled using the following pre-processor macros:
///
/// - If @p COMMON_UTILS_ENABLE_ASSERTS is defined, then @p COMMON_UTILS_ASSERT
/// is armed.
/// - If @p COMMON_UTILS_DISABLE_ASSERTS is defined, then @p COMMON_UTILS_ASSERT
/// is disarmed.
/// - If both macros are defined, then it is a compile-time error.
/// - If neither are defined, then NDEBUG governs assertions as usual.
///
/// This header will define exactly one of either @p
/// COMMON_UTILS_ASSERT_IS_ARMED or
/// @p COMMON_UTILS_ASSERT_IS_DISARMED to indicate whether @p
/// COMMON_UTILS_ASSERT is armed.
///
/// This header will define both `constexpr bool
/// common_utils::kCommonUtilsAssertIsArmed` and `constexpr bool
/// common_utils::kCommonUtilsAssertIsDisarmed` globals.
///
/// One difference versus the standard @p assert(condition) is that the
/// @p condition within @p COMMON_UTILS_ASSERT is always syntax-checked, even if
/// CommonUtils's assertions are disarmed.
///
/// Treat @p COMMON_UTILS_ASSERT like a statement -- it must always be used
/// in block scope, and must always be followed by a semicolon.
#define COMMON_UTILS_ASSERT(condition)
/// Like @p COMMON_UTILS_ASSERT, except that the expression must be void-valued;
/// this allows for guarding expensive assertion-checking subroutines using the
/// same macros as stand-alone assertions.
#define COMMON_UTILS_ASSERT_VOID(expression)
/// Evaluates @p condition and iff the value is false will trigger an assertion
/// failure with a message showing at least the condition text, function name,
/// file, and line.
#define COMMON_UTILS_DEMAND(condition)
/// Silences a "no return value" compiler warning by calling a function that
/// always raises an exception or aborts (i.e., a function marked noreturn).
/// Only use this macro at a point where (1) a point in the code is truly
/// unreachable, (2) the fact that it's unreachable is knowable from only
/// reading the function itself (and not, e.g., some larger design invariant),
/// and (3) there is a compiler warning if this macro were removed.  The most
/// common valid use is with a switch-case-return block where all cases are
/// accounted for but the enclosing function is supposed to return a value.  Do
/// *not* use this macro as a "logic error" assertion; it should *only* be used
/// to silence false positive warnings.  When in doubt, throw an exception
/// manually instead of using this macro.
#define COMMON_UTILS_UNREACHABLE()
#else  //  COMMON_UTILS_DOXYGEN_CXX

// Users should NOT set these; only this header should set them.
#ifdef COMMON_UTILS_ASSERT_IS_ARMED
#error Unexpected COMMON_UTILS_ASSERT_IS_ARMED defined.
#endif
#ifdef COMMON_UTILS_ASSERT_IS_DISARMED
#error Unexpected COMMON_UTILS_ASSERT_IS_DISARMED defined.
#endif

// Decide whether CommonUtils assertions are enabled.
#if defined(COMMON_UTILS_ENABLE_ASSERTS) && \
    defined(COMMON_UTILS_DISABLE_ASSERTS)
#error Conflicting assertion toggles.
#elif defined(COMMON_UTILS_ENABLE_ASSERTS)
#define COMMON_UTILS_ASSERT_IS_ARMED
#elif defined(COMMON_UTILS_DISABLE_ASSERTS) || defined(NDEBUG)
#define COMMON_UTILS_ASSERT_IS_DISARMED
#else
#define COMMON_UTILS_ASSERT_IS_ARMED
#endif

namespace common_utils {
namespace internal {
// Abort the program with an error message.
[[noreturn]] void Abort(const char* condition, const char* func,
                        const char* file, int line);
// Report an assertion failure; will either Abort(...) or throw.
[[noreturn]] void AssertionFailed(const char* condition, const char* func,
                                  const char* file, int line);
}  // namespace internal
namespace assert {
// Allows for specialization of how to bool-convert Conditions used in
// assertions, in case they are not intrinsically convertible.  See
// common/symbolic/expression/formula.h for an example use.  This is a public
// interface to extend; it is intended to be specialized by unusual Scalar
// types that require special handling.
template <typename Condition>
struct ConditionTraits {
  static constexpr bool is_valid = std::is_convertible_v<Condition, bool>;
  static bool Evaluate(const Condition& value) { return value; }
};
}  // namespace assert
}  // namespace common_utils

#define COMMON_UTILS_UNREACHABLE()                                            \
  ::common_utils::internal::Abort("Unreachable code was reached?!", __func__, \
                                  __FILE__, __LINE__)

#define COMMON_UTILS_DEMAND(condition)                                       \
  do {                                                                       \
    typedef ::common_utils::assert::ConditionTraits<                         \
        typename std::remove_cv_t<decltype(condition)>>                      \
        Trait;                                                               \
    static_assert(Trait::is_valid, "Condition should be bool-convertible."); \
    static_assert(                                                           \
        !std::is_pointer_v<decltype(condition)>,                             \
        "When using COMMON_UTILS_DEMAND on a raw pointer, always write out " \
        "COMMON_UTILS_DEMAND(foo != nullptr), do not write "                 \
        "COMMON_UTILS_DEMAND(foo) "                                          \
        "and rely on implicit pointer-to-bool conversion.");                 \
    if (!Trait::Evaluate(condition)) {                                       \
      ::common_utils::internal::AssertionFailed(#condition, __func__,        \
                                                __FILE__, __LINE__);         \
    }                                                                        \
  } while (0)

#ifdef COMMON_UTILS_ASSERT_IS_ARMED
// Assertions are enabled.
namespace common_utils {
constexpr bool kCommonUtilsAssertIsArmed = true;
constexpr bool kCommonUtilsAssertIsDisarmed = false;
}  // namespace common_utils
#define COMMON_UTILS_ASSERT(condition) COMMON_UTILS_DEMAND(condition)
#define COMMON_UTILS_ASSERT_VOID(expression)                         \
  do {                                                               \
    static_assert(std::is_convertible_v<decltype(expression), void>, \
                  "Expression should be void.");                     \
    expression;                                                      \
  } while (0)
#else
// Assertions are disabled, so just typecheck the expression.
namespace common_utils {
constexpr bool kCommonUtilsAssertIsArmed = false;
constexpr bool kCommonUtilsAssertIsDisarmed = true;
}  // namespace common_utils
#define COMMON_UTILS_ASSERT(condition)                                       \
  do {                                                                       \
    typedef ::common_utils::assert::ConditionTraits<                         \
        typename std::remove_cv_t<decltype(condition)>>                      \
        Trait;                                                               \
    static_assert(Trait::is_valid, "Condition should be bool-convertible."); \
    static_assert(                                                           \
        !std::is_pointer_v<decltype(condition)>,                             \
        "When using COMMON_UTILS_ASSERT on a raw pointer, always write out " \
        "COMMON_UTILS_ASSERT(foo != nullptr), do not write "                 \
        "COMMON_UTILS_ASSERT(foo) "                                          \
        "and rely on implicit pointer-to-bool conversion.");                 \
  } while (0)
#define COMMON_UTILS_ASSERT_VOID(expression)                       \
  static_assert(std::is_convertible_v<decltype(expression), void>, \
                "Expression should be void.")
#endif

#endif  // COMMON_UTILS_DOXYGEN_CXX
