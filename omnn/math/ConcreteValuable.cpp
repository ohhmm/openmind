#include "ConcreteValuable.h"
#include <stdexcept>

namespace omnn::math {

ConcreteValuable::ConcreteValuable(const std::string& str, VarHost::ptr host, bool optimized)
    : Valuable() {
    // Parse string and initialize valuable
    if (str.empty()) {
        throw std::invalid_argument("Empty string not allowed");
    }
    // TODO: Implement string parsing and initialization
    if (optimized) {
        optimize();
    }
}

ConcreteValuable::ConcreteValuable(const std::string& str, const Valuable::va_names_t& vaNames, bool optimized)
    : Valuable() {
    // Parse string with variable names
    if (str.empty()) {
        throw std::invalid_argument("Empty string not allowed");
    }
    // TODO: Implement string parsing with variable names
    if (optimized) {
        optimize();
    }
}

ConcreteValuable::ConcreteValuable(std::string_view str, const Valuable::va_names_t& vaNames, bool optimized)
    : ConcreteValuable(std::string(str), vaNames, optimized) {
}

} // namespace omnn::math
