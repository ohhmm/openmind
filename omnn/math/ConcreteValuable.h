#pragma once

#include "Valuable.h"
#include <memory>
#include <string>
#include <string_view>

namespace omnn::math {

class ConcreteValuable : public Valuable {
public:
    ConcreteValuable(const std::string& str, VarHost::ptr host, bool optimized = false);
    ConcreteValuable(const std::string& str, const Valuable::va_names_t& vaNames, bool optimized = false);
    ConcreteValuable(std::string_view str, const Valuable::va_names_t& vaNames, bool optimized = false);

    // Add other necessary methods and implementations
};

} // namespace omnn::math
