// boost_python_fix.hpp
// Fix for Boost.Python module initialization in Python 3

#ifndef OMNN_VARIABLE_BOOST_PYTHON_FIX_HPP
#define OMNN_VARIABLE_BOOST_PYTHON_FIX_HPP

#include <boost/python.hpp>

namespace boost { namespace python { namespace detail {

// Override the init_module function for Python 3
#if PY_VERSION_HEX >= 0x03000000
inline PyObject* fixed_init_module(const char* name, void(*init_function)())
{
    // Create a simple module definition
    static PyMethodDef initial_methods[] = { { nullptr, nullptr, 0, nullptr } };
    
    // Create the module definition structure
    static PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        name,
        nullptr,  // m_doc
        -1,       // m_size
        initial_methods,
        nullptr,  // m_reload
        nullptr,  // m_traverse
        nullptr,  // m_clear
        nullptr   // m_free
    };

    // Create the module using PyModule_Create
    PyObject* module = PyModule_Create(&moduledef);
    
    // Initialize the module with the init function
    if (module != nullptr) {
        // Create the current module scope
        object m_obj(((borrowed_reference_t*)module));
        scope current_module(m_obj);
        
        if (handle_exception(init_function)) {
            Py_DECREF(module);
            return nullptr;
        }
    }
    
    // Return the module object directly
    return module;
}
#endif

}}} // namespace boost::python::detail

// Macro to use the fixed init_module function
#if PY_VERSION_HEX >= 0x03000000
#define FIXED_BOOST_PYTHON_MODULE(name) \
    void init_module_##name(); \
    extern "C" BOOST_SYMBOL_EXPORT PyObject* PyInit_##name() { \
        return boost::python::detail::fixed_init_module(#name, init_module_##name); \
    } \
    void init_module_##name()
#else
#define FIXED_BOOST_PYTHON_MODULE(name) BOOST_PYTHON_MODULE(name)
#endif

#endif // OMNN_VARIABLE_BOOST_PYTHON_FIX_HPP
