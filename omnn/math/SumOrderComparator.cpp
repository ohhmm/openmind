#include "SumOrderComparator.h"
#include "Valuable.h"
#include "Sum.h"
#include "Product.h"
#include "Exponentiation.h"
#include "Variable.h"
#include "Integer.h"
#include "Fraction.h"
#include "PrincipalSurd.h"
#include "Modulo.h"
#include "Infinity.h"
#include "Euler.h"
#include "i.h"
#include "pi.h"

#include <algorithm>
#include <typeindex>

namespace {
using namespace omnn::math;
using std::type_index;

static type_index order[] = {
    typeid(NaN),
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
    typeid(Modulo),
    typeid(Infinity),
};

static auto ob = std::begin(order);
static auto oe = std::end(order);

static bool toc(const Valuable& x, const Valuable& y) {
    auto it1 = std::find(ob, oe, x.Type());
    assert(it1!=oe); // IMPLEMENT
    auto it2 = std::find(ob, oe, y.Type());
    assert(it2!=oe); // IMPLEMENT
    return it1 == it2 ? x > y : it1 < it2;
}

} // namespace

namespace omnn::math {

bool SumOrderComparator::operator()(const Valuable& v1, const Valuable& v2) const {
    if (v1.Same(v2)) {
        return false;  // Same elements are never ordered before each other
    }

    // If types are different, use type ordering
    // Handle type differences using type ordering
    if (v1.Type() != v2.Type()) {
        // Special handling for single-element collections
        if (v1.Is<Sum>()) {
            const auto& sum1 = v1.as<Sum>();
            if (sum1.size() == 1) {
                return operator()(sum1.begin()->get(), v2);
            }
        }
        if (v2.Is<Sum>()) {
            const auto& sum2 = v2.as<Sum>();
            if (sum2.size() == 1) {
                return operator()(v1, sum2.begin()->get());
            }
        }
        if (v1.Is<Product>()) {
            const auto& prod1 = v1.as<Product>();
            if (prod1.size() == 1) {
                return operator()(prod1.begin()->get(), v2);
            }
        }
        if (v2.Is<Product>()) {
            const auto& prod2 = v2.as<Product>();
            if (prod2.size() == 1) {
                return operator()(v1, prod2.begin()->get());
            }
        }

        // Use type ordering for different types
        if (!((v1.Is<Product>() && v2.Is<Exponentiation>()) ||
              (v2.Is<Product>() && v1.Is<Exponentiation>()))) {
            return toc(v1, v2);
        }
    }

    // For same types, delegate to IsComesBefore
    // This maintains antisymmetry since IsComesBefore is designed for same-type comparison
    // For same types, use IsComesBefore for consistent ordering
    if (v1.Type() == v2.Type()) {
        if (v1.IsComesBefore(v2)) {
            return true;
        }
        if (v2.IsComesBefore(v1)) {
            return false;
        }
    }

    // If neither comes before the other and they're not equal, MSVC considering it as inconsistent ordering
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
    LOG_AND_IMPLEMENT("FIXME: inconsistent ordering, SumOrderComparator failed for not equal values: " << v1 << " and " << v2);
#endif
    return {};
}

} // namespace omnn::math
