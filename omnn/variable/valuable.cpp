#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "omnn/math/Variable.h"
#include <sstream>


using namespace boost::python;
using namespace omnn::math;
using namespace std::string_literals;


BOOST_PYTHON_MODULE(variable)
{
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
        .def("__add__", +[](const Valuable& v, int i) { return v + Valuable(i); })
        .def("__radd__", +[](const Valuable& v, int i) { return Valuable(i) + v; })
        .def("__add__", +[](const Valuable& v, double d) { return v + Valuable(d); })
        .def("__radd__", +[](const Valuable& v, double d) { return Valuable(d) + v; })
        .def(self - self)
        .def("__sub__", +[](const Valuable& v, int i) { return v - Valuable(i); })
        .def("__rsub__", +[](const Valuable& v, int i) { return Valuable(i) - v; })
        .def("__sub__", +[](const Valuable& v, double d) { return v - Valuable(d); })
        .def("__rsub__", +[](const Valuable& v, double d) { return Valuable(d) - v; })
        .def(self * self)
        .def("__mul__", +[](const Valuable& v, int i) { return v * Valuable(i); })
        .def("__rmul__", +[](const Valuable& v, int i) { return Valuable(i) * v; })
        .def("__mul__", +[](const Valuable& v, double d) { return v * Valuable(d); })
        .def("__rmul__", +[](const Valuable& v, double d) { return Valuable(d) * v; })
        .def(self / self)
        .def("__truediv__", +[](const Valuable& v, int i) { return v / Valuable(i); })
        .def("__rtruediv__", +[](const Valuable& v, int i) { return Valuable(i) / v; })
        .def("__truediv__", +[](const Valuable& v, double d) { return v / Valuable(d); })
        .def("__rtruediv__", +[](const Valuable& v, double d) { return Valuable(d) / v; })
        .def(self % self)
        .def("__mod__", +[](const Valuable& v, int i) { return v % Valuable(i); })
        .def("__rmod__", +[](const Valuable& v, int i) { return Valuable(i) % v; })
        .def(self ^ self)
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
        .def(init<const std::string&>());
}
