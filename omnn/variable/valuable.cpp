#include <sstream>
#ifdef OPENMIND_BUILD_PYTHON_BINDINGS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#endif
#include "omnn/math/Variable.h"
#include "omnn/math/System.h"
#include <sstream>

#ifdef OPENMIND_BUILD_PYTHON_BINDINGS
using namespace boost::python;
#endif
using namespace omnn::math;
using namespace std::string_literals;

namespace {
#ifdef OPENMIND_BUILD_PYTHON_BINDINGS
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

    std::string variable_str(const Variable& v) {
        std::ostringstream ss;
        ss << v; // Use operator<< instead of protected print()
        return ss.str();
    }
#endif
}

#ifdef OPENMIND_BUILD_PYTHON_BINDINGS
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
                std::cout << "Converting to float: " << v << std::endl;

                // First evaluate the value
                auto result = v.evaluate();

                // If result is a Variable, try to get its stored value
                if (result.IsVa()) {
                    const auto& var = result.as<Variable>();
                    result = var.evaluate();
                }

                // Convert to double using operator double()
                return static_cast<double>(result);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Float conversion failed: ") + e.what());
            }
        })
        .def("__bool__", +[](const Valuable& v) { return static_cast<bool>(v); })

        // Evaluation
        .def("evaluate", +[](const Valuable& v) {
            std::cout << "Evaluating Valuable" << std::endl;
            return v;
        })

        .def("compile_into_lambda", +[](const Valuable& v, const boost::python::list& variables) {
            std::vector<const Variable*> vars;
            for (int i = 0; i < len(variables); ++i) {
                vars.push_back(&extract<const Variable&>(variables[i]));
            }

            return boost::python::make_function(
                [v, vars](const boost::python::list& args) -> Valuable {
                    if (len(args) != vars.size()) {
                        throw std::runtime_error("Number of arguments must match number of variables");
                    }

                    std::vector<Valuable> values;
                    for (int i = 0; i < len(args); ++i) {
                        values.push_back(extract<Valuable>(args[i]));
                    }

                    auto lambda = v.CompileIntoLambda({vars.begin(), vars.end()});
                    return lambda({values.begin(), values.end()});
                }
            );
        },
        "Create a lambda function from an expression using CompileIntoLambda.\n\n"
        "Args:\n"
        "    variables: List of Variable objects used in the expression\n\n"
        "Returns:\n"
        "    A callable function that takes a list of values corresponding to the variables\n\n"
        "Raises:\n"
        "    RuntimeError: If the number of arguments doesn't match the number of variables\n\n"
        "Example:\n"
        "    expr = x + 2 * y\n"
        "    lambda_func = expr.compile_into_lambda([x, y])\n"
        "    result = lambda_func([3, 4])  # 3 + 2*4 = 11")

        .def("compi_lambda", +[](const Valuable& v, const boost::python::list& variables) {
            std::vector<Variable> vars;
            for (int i = 0; i < len(variables); ++i) {
                vars.push_back(extract<Variable>(variables[i]));
            }

            return boost::python::make_function(
                [v, vars](const boost::python::list& args) -> Valuable {
                    if (len(args) != vars.size()) {
                        throw std::runtime_error("Number of arguments must match number of variables");
                    }

                    std::vector<Valuable> values;
                    for (int i = 0; i < len(args); ++i) {
                        values.push_back(extract<Valuable>(args[i]));
                    }

                    auto callCompiLambda = [&v](auto&&... vars) {
                        return v.CompiLambda(std::forward<decltype(vars)>(vars)...);
                    };

                    auto callLambda = [](auto&& lambda, auto&&... args) {
                        return lambda(std::forward<decltype(args)>(args)...);
                    };

                    if (vars.size() > 10) {
                        throw std::runtime_error("Currently only supports up to 10 variables due to implementation constraints");
                    }

                    auto invokeLambda = [&]() -> Valuable {
                        return [&]<size_t... I>(std::index_sequence<I...>) {
                            auto lambda = callCompiLambda(vars[I]...);

                            return callLambda(lambda, values[I]...);
                        }(std::make_index_sequence<vars.size()>{});
                    };

                    return invokeLambda();
                }
            );
        },
        "Create an optimized lambda function from an expression using CompiLambda.\n\n"
        "Args:\n"
        "    variables: List of Variable objects used in the expression (max 3 variables)\n\n"
        "Returns:\n"
        "    A callable function that takes a list of values corresponding to the variables\n\n"
        "Raises:\n"
        "    RuntimeError: If more than 3 variables are provided (C++ template limitation)\n"
        "    RuntimeError: If the number of arguments doesn't match the number of variables\n\n"
        "Note:\n"
        "    This method currently supports up to 3 variables due to C++ template limitations.\n"
        "    For expressions with more variables, use compile_into_lambda() instead.\n\n"
        "Example:\n"
        "    expr = x * y + x\n"
        "    lambda_func = expr.compi_lambda([x, y])\n"
        "    result = lambda_func([3, 4])  # 3*4 + 3 = 15")
        ;

    // Expose Variable
    class_<Variable, bases<Valuable>>("Variable")
        // Constructors
        .def(init<>())
        .def(init<const std::string&>())

        // Basic arithmetic (inherited from Valuable)
        .def(self + self)
        // Helper template for arithmetic operations
        .def("__add__", +[](const Variable& v, const Variable& other) -> Variable {
            try {
                return Variable(v.evaluate() + other.evaluate());
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Addition failed: ") + e.what());
            }
        })
        .def("__add__", +[](const Variable& v, const Valuable& other) -> Valuable {
            try {
                auto result = (v.evaluate() + other.evaluate()).evaluate();
                if (result.IsVa()) {
                    throw std::runtime_error("Addition result is still a variable");
                }

                return result;
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Addition failed: ") + e.what());
            }
        })
        .def("__add__", +[](const Variable& v, int i) -> Variable {
            return Variable(static_cast<const Valuable&>(v) + Valuable(i));
        })
        .def("__radd__", +[](const Variable& v, int i) -> Variable {
            return Variable(Valuable(i) + static_cast<const Valuable&>(v));
        })
        .def("__add__", +[](const Variable& v, double d) -> Variable {
            return Variable(static_cast<const Valuable&>(v) + Valuable(d));
        })
        .def("__radd__", +[](const Variable& v, double d) -> Variable {
            return Variable(Valuable(d) + static_cast<const Valuable&>(v));
        })
        .def(self - self)
        .def("__sub__", +[](const Variable& v, const Variable& other) {
            auto result = v - other;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__sub__", +[](const Variable& v, const Valuable& other) {
            auto result = v - other;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__sub__", +[](const Variable& v, int i) {
            auto result = v - Valuable(i);
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__rsub__", +[](const Variable& v, int i) {
            auto result = Valuable(i) - v;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__sub__", +[](const Variable& v, double d) {
            auto result = v - Valuable(d);
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__rsub__", +[](const Variable& v, double d) {
            auto result = Valuable(d) - v;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__sub__", +[](const Valuable& v, const Valuable& i) { return v - i; })
        .def("__rsub__", +[](const Valuable& v, const Valuable& i) { return i - v; })
        .def(self * self)
        .def("__mul__", +[](const Variable& v, const Variable& other) -> Variable {
            std::cout << "Variable * Variable" << std::endl;
            try {
                // First evaluate both operands
                auto v_val = v.evaluate();
                auto other_val = other.evaluate();
                std::cout << "v_val: " << v_val << ", other_val: " << other_val << std::endl;

                // Perform multiplication
                auto result = v_val * other_val;
                std::cout << "Multiplication result: " << result << std::endl;

                // Create new variable with result
                Variable var_result;
                var_result.Eval(var_result, result);

                // Verify the result
                auto final_val = var_result.evaluate();
                std::cout << "Final result: " << final_val << std::endl;

                return var_result;
            } catch (const std::exception& e) {
                std::cerr << "Exception in multiplication: " << e.what() << std::endl;
                throw;
            }
        })
        .def("__mul__", +[](const Variable& v, const Valuable& other) -> Variable {
            std::cout << "Variable * Valuable" << std::endl;
            try {
                // First evaluate the variable
                auto v_val = v.evaluate();
                std::cout << "Variable evaluated to: " << v_val << std::endl;

                // Perform multiplication
                auto result = v_val * other;
                std::cout << "Multiplication result: " << result << std::endl;

                // Create new variable with result
                Variable var_result;
                var_result.Eval(var_result, result);

                // Verify the result
                auto final_val = var_result.evaluate();
                std::cout << "Final result: " << final_val << std::endl;

                return var_result;
            } catch (const std::exception& e) {
                std::cerr << "Exception in multiplication: " << e.what() << std::endl;
                throw;
            }
        })
        .def("__mul__", +[](const Variable& v, int i) -> Variable {
            std::cout << "Variable * int" << std::endl;
            try {
                // First evaluate the variable
                auto v_val = v.evaluate();
                std::cout << "Variable evaluated to: " << v_val << std::endl;

                // Create Valuable from int
                Valuable val_i(i);
                std::cout << "Created Valuable from int: " << val_i << std::endl;

                // Perform multiplication
                auto result = v_val * val_i;
                std::cout << "Multiplication result: " << result << std::endl;

                // Create new variable with result
                Variable var_result;
                var_result.Eval(var_result, result);

                // Verify the result
                auto final_val = var_result.evaluate();
                std::cout << "Final result: " << final_val << std::endl;

                return var_result;
            } catch (const std::exception& e) {
                std::cerr << "Exception in multiplication: " << e.what() << std::endl;
                throw;
            }
        })
        .def("__rmul__", +[](const Variable& v, int i) -> Variable {
            std::cout << "int * Variable" << std::endl;
            try {
                // First evaluate the variable
                auto v_val = v.evaluate();
                std::cout << "Variable evaluated to: " << v_val << std::endl;

                // Create Valuable from int
                Valuable val_i(i);
                std::cout << "Created Valuable from int: " << val_i << std::endl;

                // Perform multiplication
                auto result = val_i * v_val;
                std::cout << "Multiplication result: " << result << std::endl;

                // Create new variable with result
                Variable var_result;
                var_result.Eval(var_result, result);

                // Verify the result
                auto final_val = var_result.evaluate();
                std::cout << "Final result: " << final_val << std::endl;

                return var_result;
            } catch (const std::exception& e) {
                std::cerr << "Exception in multiplication: " << e.what() << std::endl;
                throw;
            }
        })
        .def("__mul__", +[](const Variable& v, double d) -> Valuable {
            std::cout << "Variable * double" << std::endl;
            try {
                // First evaluate the variable
                auto v_val = v.evaluate();
                std::cout << "Variable evaluated to: " << v_val << std::endl;

                // Create Valuable from double
                Valuable val_d(d);
                std::cout << "Created Valuable from double: " << val_d << std::endl;

                // Perform multiplication and evaluate immediately
                auto result = (v_val * val_d).evaluate();
                std::cout << "Final result: " << result << std::endl;

                if (result.IsVa()) {
                    throw std::runtime_error("Multiplication result is still a variable");
                }

                return result;
            } catch (const std::exception& e) {
                std::cerr << "Exception in multiplication: " << e.what() << std::endl;
                throw;
            }
        })
        .def("__rmul__", +[](const Variable& v, double d) -> Variable {
            std::cout << "double * Variable" << std::endl;
            try {
                auto result = Valuable(d) * static_cast<const Valuable&>(v);
                std::cout << "Multiplication successful" << std::endl;
                return Variable(result);
            } catch (const std::exception& e) {
                std::cerr << "Exception in multiplication: " << e.what() << std::endl;
                throw;
            }
        })
        .def(self / self)
        .def("__truediv__", +[](const Variable& v, const Variable& other) {
            auto result = v / other;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__truediv__", +[](const Variable& v, const Valuable& other) {
            auto result = v / other;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__truediv__", +[](const Variable& v, int i) {
            auto result = v / Valuable(i);
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__rtruediv__", +[](const Variable& v, int i) {
            auto result = Valuable(i) / v;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__truediv__", +[](const Variable& v, double d) {
            auto result = v / Valuable(d);
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
        .def("__rtruediv__", +[](const Variable& v, double d) {
            auto result = Valuable(d) / v;
            return result.IsVa() ? result.as<Variable>() : Variable(result);
        })
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
            std::cout << "Setting double value: " << value << std::endl;
            try {
                // Create a Valuable with the value
                Valuable val(value);

                // Store value using public Eval method
                v.Eval(v, val);

                // Verify the value was set
                auto result = v.evaluate();
                std::cout << "Value set, evaluates to: " << result << std::endl;

                if (result.IsVa()) {
                    throw std::runtime_error("Failed to set value - still a variable after assignment");
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception in set_value: " << e.what() << std::endl;
                throw;
            }
        })
        .def("set_value", +[](Variable& v, int i) {
            std::cout << "Setting int value: " << i << std::endl;
            v.Eval(v, Valuable(i));
            // Verify the value was set
            auto result = v.evaluate();
            std::cout << "Value set, evaluates to: " << result << std::endl;
        })
        .def("set_value", +[](Variable& v, double d) {
            std::cout << "Setting double value: " << d << std::endl;
            v.Eval(v, Valuable(d));
            // Verify the value was set
            auto result = v.evaluate();
            std::cout << "Value set, evaluates to: " << result << std::endl;
        })
        .def("evaluate", +[](Variable& v) {
            std::cout << "Evaluating Variable expression" << std::endl;
            try {
                // Evaluate using Variable's evaluate()
                auto result = v.evaluate();
                std::cout << "Got result from evaluate(): " << result << std::endl;

                // If we got a variable result, try to get its concrete value
                if (result.IsVa()) {
                    const auto& var = result.as<Variable>();
                    auto concrete = var.evaluate();
                    if (!concrete.IsVa()) {
                        result = concrete;
                        std::cout << "Found concrete value: " << result << std::endl;
                    }
                }

                return result;
            } catch (const std::exception& e) {
                std::cerr << "Exception in evaluate: " << e.what() << std::endl;
                throw;
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
#endif // OPENMIND_BUILD_PYTHON_BINDINGS
