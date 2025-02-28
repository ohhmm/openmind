#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "boost_python_fix.hpp"  // Include the custom header file
#include "omnn/math/Variable.h"
#include <sstream>
#include <map>

using namespace boost::python;
using namespace omnn::math;
using namespace std::string_literals;

namespace {
    // Converter function
    std::map<omnn::math::Variable, omnn::math::Valuable> dict_to_map(const dict& py_dict) {
        std::map<omnn::math::Variable, omnn::math::Valuable> cpp_map;
        list keys = py_dict.keys();
        for (int i = 0; i < len(keys); ++i) {
            cpp_map[extract<omnn::math::Variable>(keys[i])] = extract<omnn::math::Valuable>(py_dict[keys[i]]);
        }
        return cpp_map;
    }

    // Register the converter
    struct dict_to_map_converter {
        dict_to_map_converter() {
            converter::registry::push_back(
                &convertible,
                &construct,
                type_id<std::map<omnn::math::Variable, omnn::math::Valuable>>());
        }

        // Check if the object can be converted
        static void* convertible(PyObject* obj_ptr) {
            return PyDict_Check(obj_ptr) ? obj_ptr : nullptr;
        }

        // Perform the conversion
        static void construct(PyObject* obj_ptr, converter::rvalue_from_python_stage1_data* data) {
            void* storage = ((converter::rvalue_from_python_storage<std::map<omnn::math::Variable, omnn::math::Valuable>>*)data)->storage.bytes;
            // Use handle<> without .ptr() to ensure proper reference counting
            new (storage) std::map<omnn::math::Variable, omnn::math::Valuable>(dict_to_map(dict(handle<>(borrowed(obj_ptr)))));
            data->convertible = storage;
        }
    };
}

// Use the fixed module initialization macro
FIXED_BOOST_PYTHON_MODULE(variable)
{
    dict_to_map_converter();  // Register the converter

    // Expose View enum
    enum_<Valuable::View>("View")
        .value("None", Valuable::View::None)
        .value("Calc", Valuable::View::Calc)
        .value("Condensed", Valuable::View::Condensed)
        .value("Equation", Valuable::View::Equation)
        .value("Flat", Valuable::View::Flat)
        .value("Fraction", Valuable::View::Fraction)
        .value("Solving", Valuable::View::Solving)
        .value("SupersetOfRoots", Valuable::View::SupersetOfRoots);

    // Expose Valuable
    class_<Valuable>("Valuable")
        // Constructors
        .def(init<>())
        .def(init<int>())
        .def(init<double>())
        .def(init<const a_int&>())
        .def(init<const a_rational&>())
        .def(init<const std::string&>())
        
        // Basic arithmetic with numeric conversion
        .def(self + self)
        .def("__add__", +[](const Valuable& v, int i) { return v + i; })
        .def("__radd__", +[](const Valuable& v, int i) { return i + v; })
        .def("__add__", +[](const Valuable& v, double d) { return v + d; })
        .def("__radd__", +[](const Valuable& v, double d) { return d + v; })
        .def("__add__", +[](const Valuable& v, const Variable& d) { return v + d; })
        .def("__radd__", +[](const Valuable& v, const Variable& d) { return d + v; })
        .def(self - self)
        .def("__sub__", +[](const Valuable& v, int i) { return v - i; })
        .def("__rsub__", +[](const Valuable& v, int i) { return i - v; })
        .def("__sub__", +[](const Valuable& v, double d) { return v - d; })
        .def("__rsub__", +[](const Valuable& v, double d) { return d - v; })
        .def(self * self)
        .def("__mul__", +[](const Valuable& v, int i) { return v * i; })
        .def("__rmul__", +[](const Valuable& v, int i) { return i * v; })
        .def("__mul__", +[](const Valuable& v, double d) { return v * d; })
        .def("__rmul__", +[](const Valuable& v, double d) { return d * v; })
        .def(self / self)
        .def("__truediv__", +[](const Valuable& v, int i) { return v / i; })
        .def("__rtruediv__", +[](const Valuable& v, int i) { return i / v; })
        .def("__truediv__", +[](const Valuable& v, double d) { return v / d; })
        .def("__rtruediv__", +[](const Valuable& v, double d) { return d / v; })
        .def(self % self)
        .def("__mod__", +[](const Valuable& v, int i) { return v % i; })
        .def("__rmod__", +[](const Valuable& v, int i) { return i % v; })
        .def(self ^ self)
        .def("__pow__", +[](const Valuable& v, int i) { return v ^ i; })
        .def("__rpow__", +[](const Valuable& v, int i) { return i ^ v; })
        .def("__pow__", +[](const Valuable& v, double d) { return v ^ d; })
        .def("__rpow__", +[](const Valuable& v, double d) { return d ^ v; })
        .def(-self)

        // Comparison
        .def(self < self)
        .def(self == self)
        .def(self != self)

        // Bit operations
        .def("shl", +[](const Valuable& v, int shift) { return v.Shl(shift); })
        .def("shr", +[](const Valuable& v, int shift) { return v.Shr(shift); })
        .def("bit", +[](const Valuable& v, int bit) { return v.bit(bit); })

        // Mathematical functions
        .def("abs", &Valuable::Abs)
        .def("sq", &Valuable::Sq)
        .def("sqrt", &Valuable::Sqrt)
        .def("optimize", &Valuable::optimize)
        .def("eval", &Valuable::eval)

        // Type conversion
        .def("__int__", +[](const Valuable& v) { return static_cast<int>(v); })
        .def("__float__", +[](const Valuable& v) { return static_cast<double>(v); })

        // String conversion
        .def("__str__", &Valuable::str)
        ;

    class_<Variable, bases<Valuable>>("Variable")
        // Constructors
        .def(init<>())
        .def(init<const std::string&>())

        // Basic arithmetic with numeric conversion
        .def(self + self)
        .def("__add__", +[](const Variable& v, int i) { return v + i; })
        .def("__radd__", +[](const Variable& v, int i) { return i + v; })
        .def("__add__", +[](const Variable& v, double d) { return v + d; })
        .def("__radd__", +[](const Variable& v, double d) { return d + v; })
        .def(self - self)
        .def("__sub__", +[](const Variable& v, int i) { return v - i; })
        .def("__rsub__", +[](const Variable& v, int i) { return i - v; })
        .def("__sub__", +[](const Variable& v, double d) { return v - d; })
        .def("__rsub__", +[](const Variable& v, double d) { return d - v; })
        .def(self * self)
        .def("__mul__", +[](const Variable& v, int i) { return v * i; })
        .def("__rmul__", +[](const Variable& v, int i) { return i * v; })
        .def("__mul__", +[](const Variable& v, double d) { return v * d; })
        .def("__rmul__", +[](const Variable& v, double d) { return d * v; })
        .def(self / self)
        .def("__truediv__", +[](const Variable& v, int i) { return v / i; })
        .def("__rtruediv__", +[](const Variable& v, int i) { return i / v; })
        .def("__truediv__", +[](const Variable& v, double d) { return v / d; })
        .def("__rtruediv__", +[](const Variable& v, double d) { return d / v; })
        .def(self % self)
        .def("__mod__", +[](const Variable& v, int i) { return v % i; })
        .def("__rmod__", +[](const Variable& v, int i) { return i % v; })
        .def(self ^ self)
        .def("__pow__", +[](const Variable& v, int i) { return v ^ i; })
        .def("__rpow__", +[](const Variable& v, int i) { return i ^ v; })
        .def("__pow__", +[](const Variable& v, double d) { return v ^ d; })
        .def("__rpow__", +[](const Variable& v, double d) { return d ^ v; })
        .def(-self)

        // Comparison
        .def(self < self)
        .def(self == self)
        .def(self != self)
        ;
}
