#include "ProductOrderComparator.h"

#include "Valuable.h"
#include "Variable.h"
#include "Integer.h"
#include "Infinity.h"
#include "Modulo.h"
#include "PrincipalSurd.h"
#include "pi.h"
#include "i.h"
#include "Sum.h"
#include "Euler.h"
#include "Exponentiation.h"
#include "Logarithm.h"

#include <algorithm>
#include <typeindex>


using namespace omnn::math;


//static
ProductOrderComparator::index_t ProductOrderComparator::Order(const Valuable& value) {
    static const std::type_index order[] = {
        // for fast optimizing
        typeid(NaN),
        typeid(MInfinity),
        typeid(Infinity),
        typeid(Sum),
        typeid(Product),
        // general order
        typeid(Integer),
        typeid(MinusOneSurd),
        typeid(Euler),
        typeid(Pi),
        typeid(Fraction),
        typeid(PrincipalSurd),
        typeid(Exponentiation),
        typeid(Logarithm),
        typeid(Variable),
        typeid(Modulo),
    };

    static auto ob = std::cbegin(order);
    static auto oe = std::cend(order);

    auto it = std::find(ob, oe, value.Type());
    if (it == oe)
        LOG_AND_IMPLEMENT("Introduce new type to Product ordering:" << value);
    return it - ob;
}

bool ProductOrderComparator::operator()(const Valuable& x, const Valuable& y) const {
    auto it1 = Order(x);
    auto it2 = Order(y);
    return it1 == it2 ? x.IsComesBefore(y) : it1 < it2;
}
