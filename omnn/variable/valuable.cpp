#include <sstream>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "omnn/math/Variable.h"
#include "omnn/math/System.h"
#include <sstream>

using namespace boost::python;
using namespace omnn::math;
using namespace std::string_literals;

namespace {
    // Converter for solutions_t (std::unordered_set<Valuable>)
    boost::python::list solutions_to_list(const omnn::math::Valuable::solutions_t& solutions) {
        boost::python::list result;
        for (const auto& solution : solutions) {
            result.append(solution);
        }
        return result;
    }

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
            new (storage) std::map<omnn::math::Variable, omnn::math::Valuable>(dict_to_map(dict(handle<>(borrowed(obj_ptr)))));
            data->convertible = storage;
        }
    };

    // Helper for __str__
    std::string valuable_str(const Valuable& v) {
        std::ostringstream ss;
        ss << v; // Use operator<< instead of protected print()
        return ss.str();
    }
// Helper functions for arithmetic operations
Variable add_variables(const Variable& v1, const Variable& v2) {
    return Variable(v1.evaluate() + v2.evaluate());
}

Variable add_variable_valuable(const Variable& v, const Valuable& val) {
    return Variable(v.evaluate() + val);
}

Variable add_variable_int(const Variable& v, int i) {
    return Variable(v.evaluate() + Valuable(i));
}

Variable radd_variable_int(const Variable& v, int i) {
    return Variable(Valuable(i) + v.evaluate());
}

Variable add_variable_double(const Variable& v, double d) {
    return Variable(v.evaluate() + Valuable(d));
}

Variable radd_variable_double(const Variable& v, double d) {
    return Variable(Valuable(d) + v.evaluate());
}

Variable sub_variables(const Variable& v1, const Variable& v2) {
    return Variable(v1.evaluate() - v2.evaluate());
}

Variable sub_variable_valuable(const Variable& v, const Valuable& val) {
    return Variable(v.evaluate() - val);
}

Variable sub_variable_int(const Variable& v, int i) {
    return Variable(v.evaluate() - Valuable(i));
}

Variable rsub_variable_int(const Variable& v, int i) {
    return Variable(Valuable(i) - v.evaluate());
}

Variable sub_variable_double(const Variable& v, double d) {
    return Variable(v.evaluate() - Valuable(d));
}

Variable rsub_variable_double(const Variable& v, double d) {
    return Variable(Valuable(d) - v.evaluate());
}

Variable mul_variables(const Variable& v1, const Variable& v2) {
    return Variable(v1.evaluate() * v2.evaluate());
}

Variable mul_variable_valuable(const Variable& v, const Valuable& val) {
    return Variable(v.evaluate() * val);
}

Variable mul_variable_int(const Variable& v, int i) {
    return Variable(v.evaluate() * Valuable(i));
}

Variable rmul_variable_int(const Variable& v, int i) {
    return Variable(Valuable(i) * v.evaluate());
}

Variable mul_variable_double(const Variable& v, double d) {
    return Variable(v.evaluate() * Valuable(d));
}

Variable rmul_variable_double(const Variable& v, double d) {
    return Variable(Valuable(d) * v.evaluate());
}

Variable div_variables(const Variable& v1, const Variable& v2) {
    return Variable(v1.evaluate() / v2.evaluate());
}

Variable div_variable_valuable(const Variable& v, const Valuable& val) {
    return Variable(v.evaluate() / val);
}

Variable div_variable_int(const Variable& v, int i) {
    return Variable(v.evaluate() / Valuable(i));
}

Variable rdiv_variable_int(const Variable& v, int i) {
    return Variable(Valuable(i) / v.evaluate());
}

Variable div_variable_double(const Variable& v, double d) {
    return Variable(v.evaluate() / Valuable(d));
}

Variable rdiv_variable_double(const Variable& v, double d) {
    return Variable(Valuable(d) / v.evaluate());
}

// Helper for __str__
std::string valuable_str(const Valuable& v) {
    std::ostringstream ss;
    ss << v;  // Use operator<< instead of protected print()
    return ss.str();
}

}

BOOST_PYTHON_MODULE(variable)
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
        .def("__le__", +[](const Valuable& a, const Valuable& b) -> bool { return a < b || a == b; })
        .def("__gt__", +[](const Valuable& a, const Valuable& b) -> bool { return !(a < b || a == b); })
        .def("__ge__", +[](const Valuable& a, const Valuable& b) -> bool { return !(a < b); })
        .def(self == self)
        .def(self != self)

        // Bit operations
        .def("shl", +[](const Valuable& v, int shift) { return v.Shl(shift); })
        .def("shr", +[](const Valuable& v, int shift) { return v.Shr(shift); })
        .def("bit", +[](const Valuable& v, int bit) { return v.bit(bit); })
        .def("and_", +[](const Valuable& v, const Valuable& other) { return v.And(v, other); })
        .def("or_", +[](const Valuable& v, const Valuable& other) { return v.Or(v, other); })
        .def("xor_", +[](const Valuable& v, const Valuable& other) { return v.Xor(v, other); })
        .def("not_", +[](const Valuable& v) { return v.Not(v); })

        // Mathematical functions
        .def("abs", &Valuable::Abs)
        .def("sq", &Valuable::Sq)
        .def("sqrt", &Valuable::Sqrt)
        .def("optimize", &Valuable::optimize)
        // Evaluation
        .def("eval", &Valuable::eval)

        // Type identification
        .def("is_constant", &Valuable::IsConstant)
        .def("is_int", &Valuable::IsInt)
        .def("is_fraction", &Valuable::IsFraction)
        .def("is_zero", &Valuable::IsZero)
        .def("is_rational", &Valuable::IsRational)

        // View system
        .def("get_view", &Valuable::GetView)
        .def("set_view", &Valuable::SetView)

        // String conversion
        .def("__str__", &Valuable::str)

        // Type conversion
        .def("__int__", +[](const Valuable& v) { return static_cast<int>(v); })
        .def("__float__", +[](const Valuable& v) {
            try {
                return static_cast<double>(v);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Float conversion failed: ") + e.what());
            }
        })
        .def("__bool__", +[](const Valuable& v) { return static_cast<bool>(v); })
        ;


    // Expose Variable
    class_<Variable, bases<Valuable>>("Variable")
        // Constructors
        .def(init<>())
        .def(init<const std::string&>())

        // Basic arithmetic (inherited from Valuable)
        .def(self + self)
        .def("__add__", add_variables)
        .def("__add__", add_variable_valuable)
        .def("__add__", add_variable_int)
        .def("__radd__", radd_variable_int)
        .def("__add__", add_variable_double)
        .def("__radd__", radd_variable_double)
        .def(self - self)
        .def("__sub__", sub_variables)
        .def("__sub__", sub_variable_valuable)
        .def("__sub__", sub_variable_int)
        .def("__rsub__", rsub_variable_int)
        .def("__sub__", sub_variable_double)
        .def("__rsub__", rsub_variable_double)
        .def(self * self)
        // Multiplication operations
        .def("__mul__", mul_variables)
        .def("__mul__", mul_variable_valuable)
        .def("__mul__", mul_variable_int)
        .def("__rmul__", rmul_variable_int)
        .def("__mul__", mul_variable_double)
        .def("__rmul__", rmul_variable_double)
        .def(self / self)
        // Division operations
        .def("__truediv__", div_variables)
        .def("__truediv__", div_variable_valuable)
        .def("__truediv__", div_variable_int)
        .def("__rtruediv__", rdiv_variable_int)
        .def("__truediv__", div_variable_double)
        .def("__rtruediv__", rdiv_variable_double)
        .def(self % self)
        .def("__mod__", +[](const Variable& v, const Variable& other) { return Variable(v % other); })
        .def("__mod__", +[](const Variable& v, const Valuable& other) { return Variable(v % other); })
        .def("__mod__", +[](const Variable& v, int i) { return Variable(v % Valuable(i)); })
        .def("__rmod__", +[](const Variable& v, int i) { return Variable(Valuable(i) % v); })
        .def(self ^ self)
        .def(-self)

        // Comparison (inherited from Valuable)
        .def(self < self)
        .def("__le__", +[](const Variable& a, const Variable& b) -> bool { return a < b || a == b; })
        .def("__gt__", +[](const Variable& a, const Variable& b) -> bool { return !(a < b || a == b); })
        .def("__ge__", +[](const Variable& a, const Variable& b) -> bool { return !(a < b); })
        .def(self == self)
        .def(self != self)

        // String conversion
        .def("__str__", &variable_str)

        // Type identification
        .def("is_va", &Variable::IsVa)

        // Variable-specific methods
        .def("set_value", +[](Variable& v, double value) {
            v.Eval(v, Valuable(value));
            // Verify the value was set
                auto result = v.evaluate();
                std::cout << "Value set, evaluates to: " << result << std::endl;

                if (result.IsVa()) {
                    throw std::runtime_error("Failed to set value - still a variable after assignment");
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Failed to set value: ") + e.what());
            }
        })
        .def("set_value", +[](Variable& v, int i) {
            try {
                v.Eval(v, Valuable(i));auto result = v.evaluate();
            if (result.IsVa()) {
                throw std::runtime_error("Failed to set value - still a variable after assignment");
            }
        })
        .def("evaluate", +[](Variable& v) -> Valuable {
            try {
                return v.evaluate();
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Evaluation failed: ") + e.what());
            }
        });

    class_<System>("System")
        // Constructors
        .def(init<>())
        .def(init<const boost::numeric::ublas::matrix<Valuable>&>())

        // Methods
        .def("Add", static_cast<bool (System::*)(const Valuable&)>(&System::Add))
        .def("Add", static_cast<bool (System::*)(const Variable&, const Valuable&)>(&System::Add))
        .def("Eval", &System::Eval)
        .def("Fetch", &System::Fetch)
        .def("Has", &System::Has)
        .def("Solve", +[](System& sys, const Variable& va) {
            return solutions_to_list(sys.Solve(va));
        })
        .def("Vars", +[](const System& sys) {
            boost::python::list result;
            for (const auto& var : sys.Vars()) {
                result.append(var);
            }
            return result;
        })
        .def("Test", +[](const System& sys, const Valuable::vars_cont_t& vars) {
            return sys.Test(vars);
        })
        .def("MakesTotalEqu", static_cast<void (System::*)(bool)>(&System::MakesTotalEqu))
        .def("CalculateTotalExpression", &System::CalculateTotalExpression)
        .def("IsEmpty", &System::IsEmpty)

        // Operators
        .def("__str__", +[](const System& sys) {
            std::ostringstream ss;
            ss << "System with " << sys.size() << " equations";
            return ss.str();
        })
        .def("__lshift__", +[](System& sys, const Valuable& val) -> System& {
            return sys << val;
        }, return_internal_reference<>())
        ;
}
