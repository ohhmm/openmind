#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Valuable.h"
#include "Variable.h"

namespace py = pybind11;

PYBIND11_MODULE(valuable_bindings, m) {
    py::class_<omnn::math::Valuable>(m, "Valuable")
        .def(py::init<>())  // Default constructor
        .def(py::init<const std::string&, omnn::math::Valuable::NewVaFn_t>(), py::arg("str"), py::arg("newVa"))  // Constructor with string argument and NewVaFn_t
        .def("LoadONNXModel", &omnn::math::Valuable::LoadONNXModel)
        .def("RunONNXInference", &omnn::math::Valuable::RunONNXInference)
        .def("add_conv", &omnn::math::Valuable::add_conv)
        .def("add_relu", &omnn::math::Valuable::add_relu)
        .def("add_add", &omnn::math::Valuable::add_add)
        .def("Type", &omnn::math::Valuable::Type)
        .def("Link", &omnn::math::Valuable::Link)
        .def("SerializedStrEqual", &omnn::math::Valuable::SerializedStrEqual)
        .def("GCD", &omnn::math::Valuable::GCD)
        .def("LCM", &omnn::math::Valuable::LCM)
        .def("integral", py::overload_cast<const omnn::math::Variable&, const omnn::math::Variable&>(&omnn::math::Valuable::integral))
        .def("integral", py::overload_cast<const omnn::math::Variable&, const omnn::math::Valuable&, const omnn::math::Valuable&, const omnn::math::Variable&>(&omnn::math::Valuable::integral))
        .def("Integral", py::overload_cast<const omnn::math::Variable&, const omnn::math::Variable&>(&omnn::math::Valuable::Integral, py::const_))
        .def("Integral", py::overload_cast<const omnn::math::Variable&, const omnn::math::Valuable&, const omnn::math::Valuable&, const omnn::math::Variable&>(&omnn::math::Valuable::Integral, py::const_))
        .def("operator=", py::overload_cast<const omnn::math::Valuable&>(&omnn::math::Valuable::operator=))
        .def("operator=", [](omnn::math::Valuable& self, omnn::math::Valuable other) { return self = std::move(other); });
}
