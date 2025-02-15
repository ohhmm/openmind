#include "pi.h"
#include <numbers>

using namespace omnn::math;

constinit std::string_view Pi::SerializationName = "pi";

bool Pi::operator<(const Valuable& v) const {
    if (v.IsInt()) {
        return v > 3;
    } else {
        return base::operator<(v);
    }
}

Valuable Pi::Sign() const {
    return Valuable(std::static_pointer_cast<Valuable>(std::make_shared<Integer>(1)));
}

Pi::operator double() const { return std::numbers::pi; }
