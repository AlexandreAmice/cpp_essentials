
#include "pybind/copyable_unique_ptr_holder_type.h"
#include "utils/random.h"

namespace py = pybind11;

using namespace common_utils;
void AddUtilsBindings(py::module_& m) {
  py::class_<RandomGenerator>(m, "RandomGenerator")
      .def(py::init<>())
      .def(py::init<RandomGenerator::result_type>(), py::arg("seed"))
      .def(
          "__call__",
          [](RandomGenerator& self) {
            return self();
          },
          "Generates a pseudo-random value.");
}
