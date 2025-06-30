#include <pybind11/eigen.h>  // Common for Eigen types
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // Common for STL containers

namespace py = pybind11;

// Forward declarations of binding functions from other .cc files
// These functions will be defined in their respective *_bindings.cc files
void AddUtilsBindings(py::module_& m);

// The name common_utils MUST be the same as name as the name of the
// common_utils_pybind_extension. It also must be the same options variable in
// the generate_stubs.py file.
PYBIND11_MODULE(cpp_essentials, m) {
  m.doc() = "Python bindings for common_utils library";

  AddUtilsBindings(m);
}
