#include "utils/copyable_unique_ptr.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, common_utils::copyable_unique_ptr<T>);
