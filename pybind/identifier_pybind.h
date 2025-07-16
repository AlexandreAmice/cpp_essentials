#pragma once

#include <string>

#include "pybind/pymodule_pybind.h"
#include "utils/identifier.h"

namespace common_utils::py_common_utils {

/// Binds an Identifier instantiation.
template <typename Class, typename ModuleOrClass>
void BindIdentifier(ModuleOrClass m, const std::string& name,
                    const char* id_doc) {
  py::class_<Class>(m, name.c_str(), id_doc)
      .def("get_value", &Class::get_value)
      .def("is_valid", &Class::is_valid)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::hash(py::self))
      .def_static("get_new_id", &Class::get_new_id)
      .def("__repr__", [name](const Class& self) {
        return py::str("<{} value={}>").format(name, self.get_value());
      });
}

}  // namespace common_utils::py_common_utils
