#include "SumOrderComparator.h"
#include "Valuable.h"
#include "Sum.h"
#include "Product.h"
#include "Exponentiation.h"
#include "Variable.h"
#include "Integer.h"
#include "Fraction.h"
#include "Logarithm.h"
#include "PrincipalSurd.h"
#include "Modulo.h"
#include "Infinity.h"
#include "Euler.h"
#include "i.h"
#include "pi.h"
#include "NaN.h"

#include <algorithm>
#include <typeindex>

namespace omnn::math {

namespace {
    // inequality should cover all cases
    bool type_order_comparator(const Valuable& x, const Valuable& y) // type order comparator
    {
        using namespace std;
        static type_index order[] = {
            typeid(omnn::math::NaN),
            typeid(MInfinity),
            typeid(Sum),
            typeid(Product),
            typeid(Exponentiation),
            typeid(Variable),
            typeid(Euler),
            typeid(Pi),
            typeid(MinusOneSurd),
            typeid(Integer),
            typeid(Fraction),
            typeid(PrincipalSurd),
            typeid(Logarithm),
            typeid(Modulo),
            typeid(Infinity),
        };

        static auto ob = std::begin(order);
        static auto oe = std::end(order);

        auto it1 = std::find(ob, oe, x.Type());
        assert(it1!=oe); // IMPLEMENT
        auto it2 = std::find(ob, oe, y.Type());
        assert(it2!=oe); // IMPLEMENT
        return it1 == it2 ? x > y : it1 < it2;
    }
}

const SumOrderComparator::TypeOrderComparator SumOrderComparator::toc = type_order_comparator;

bool SumOrderComparator::operator()(const Valuable& v1, const Valuable& v2) const
{
    // Handle NaN cases first
    if (v1.IsNaN() || v2.IsNaN()) {
        return false;  // NaN is not ordered before anything
    }

    // Handle equality cases - must be before NaN check to ensure correct behavior
    if (v1.Same(v2)) {
        return false; // Same elements are never ordered before each other
    }

    // For numeric types, handle direct comparison
    if (v1.IsNumeric() && v2.IsNumeric()) {
        // For same numeric values, return false (not less than)
        if (v1 == v2) {
            return false;
        }
        // For different numeric values, compare directly
        // Convert to numeric values for comparison to handle NaN correctly
        auto val1 = v1.IsNaN() ? std::numeric_limits<double>::quiet_NaN() : v1.ToBool();
        auto val2 = v2.IsNaN() ? std::numeric_limits<double>::quiet_NaN() : v2.ToBool();
        return val1 < val2;
    }

    // If types are different, use type ordering
    if (v1.Type() != v2.Type()) {
        // Handle single-element collections
        if (v1.IsSum() && v1.as<Sum>().size() == 1) {
            return operator()(v1.as<Sum>().begin()->get(), v2);
        }
        if (v2.IsSum() && v2.as<Sum>().size() == 1) {
            return operator()(v1, v2.as<Sum>().begin()->get());
        }
        if (v1.IsProduct() && v1.as<Product>().size() == 1) {
            return operator()(v1.as<Product>().begin()->get(), v2);
        }
        if (v2.IsProduct() && v2.as<Product>().size() == 1) {
            return operator()(v1, v2.as<Product>().begin()->get());
        }

        // Special case for Product-Exponentiation comparison
        if (!((v1.IsProduct() && v2.IsExponentiation())
              || (v2.IsProduct() && v1.IsExponentiation()))) {
            return toc(v1, v2);
        }
    }

    // For same types or special cases, delegate to IsComesBefore
    if (v1.IsComesBefore(v2)) {
        return true;
    }
    if (v2.IsComesBefore(v1)) {
        return false;
    }

    // If we can't determine order through IsComesBefore, use string representation
    // This ensures a consistent total ordering
    return v1.str() < v2.str();
}

} // namespace omnn::math
