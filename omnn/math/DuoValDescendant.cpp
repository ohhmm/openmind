/*
 * DuoValDescendant.h
 *
 *  Created on: Sep 30, 2018
 *      Author: sergejkrivonos
 */

#include "DuoValDescendant.h"
#include "Product.h"

namespace omnn::math {

namespace{
bool NoNeedBraces(const Valuable& v){
    return (v.IsInt() && v>=0) || v.IsVa() || v.IsSum();
}
}

std::ostream& PrintVal(std::ostream& out, const Valuable& v)
{
    if(!NoNeedBraces(v))
        out << '(' << v << ')';
    else
        out << v;

    return out;
}

std::ostream& PrintCodeVal(std::ostream& out, const Valuable& v)
{
    if(!NoNeedBraces(v))
        v.code(out << '(') << ')';
    else
        v.code(out);

    return out;
}

Valuable Multiply(const Valuable& _1, const Valuable& _2) {
    return Product{_1, _2};
}

} // omnn::math
