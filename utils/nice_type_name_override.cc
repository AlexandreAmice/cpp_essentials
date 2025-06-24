#include "utils/nice_type_name_override.h"

#include "utils/common_utils_assert.h"
#include "utils/never_destroyed.h"
/**
 * This file is copied from Drake's common/nice_type_name_override.cc
 * implementation with appropriate modifications.
 */
namespace common_utils {
namespace internal {

namespace {
NiceTypeNamePtrOverride& ptr_override() {
  static never_destroyed<NiceTypeNamePtrOverride> value;
  return value.access();
}
}  // namespace

void SetNiceTypeNamePtrOverride(NiceTypeNamePtrOverride new_ptr_override) {
  COMMON_UTILS_DEMAND(ptr_override() == nullptr);
  COMMON_UTILS_DEMAND(new_ptr_override != nullptr);
  ptr_override() = new_ptr_override;
}

const NiceTypeNamePtrOverride& GetNiceTypeNamePtrOverride() {
  return ptr_override();
}

}  // namespace internal
}  // namespace common_utils
