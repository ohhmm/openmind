/*
 * DuoValDescendant.h
 *
 *  Created on: Sep 30, 2018
 *      Author: sergejkrivonos
 */

#include "DuoValDescendant.h"

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

} // omnn::math
