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

Pi::operator double() const { return std::numbers::pi; }
