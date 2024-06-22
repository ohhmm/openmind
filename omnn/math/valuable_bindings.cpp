#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Valuable.h"

namespace py = pybind11;

PYBIND11_MODULE(valuable_bindings, m) {
    py::class_<omnn::math::Valuable>(m, "Valuable")
        .def(py::init<>())  // Default constructor
        .def(py::init<const std::string&, omnn::math::Valuable::NewVaFn_t>(), py::arg("str"), py::arg("newVa"))  // Constructor with string argument and NewVaFn_t
        .def("LoadONNXModel", &omnn::math::Valuable::LoadONNXModel)
        .def("RunONNXInference", &omnn::math::Valuable::RunONNXInference)
        .def("add_conv", &omnn::math::Valuable::add_conv)
        .def("add_relu", &omnn::math::Valuable::add_relu)
        .def("add_add", &omnn::math::Valuable::add_add);
}
