#pragma once

#include <stdexcept>
#include <string>

/**
 * This file is copied from Drake's drake/common/drake_assertion_error.h with
 * appropriate modifications.
 */
namespace common_utils {
namespace internal {

// This is what COMMON_UTILS_ASSERT and COMMON_UTILS_DEMAND throw when our assertions are
// configured to throw.
class assertion_error : public std::runtime_error {
 public:
  explicit assertion_error(const std::string& what_arg)
      : std::runtime_error(what_arg) {}
};

}  // namespace internal
}  // namespace common_utils
