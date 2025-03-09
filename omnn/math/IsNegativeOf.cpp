#include "Integer.h"
#include "Variable.h"

namespace omnn {
namespace math {

Valuable Integer::IsNegativeOf(const Valuable& other) const
{
    if (other.IsInt()) {
        return (arbitrary < 0 && arbitrary == -other.ca()) ? Integer(1) : Integer(0);
    }
    else if (other.IsFraction()) {
        return Integer(0);
    }
    else {
        return Integer(0);
    }
}

} // namespace math
} // namespace omnn
