/*
 * Modulo.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#include <omnn/math/Modulo.h>

using namespace omnn::math;

std::ostream& Modulo::print_sign(std::ostream& out) const
{
    return out << " mod ";
}

void Modulo::optimize(){
    _1.optimize();
    _2.optimize();
    if (_1.IsInt() && _2.IsInt()) {
        if (_2 == constants::one)
            Become(std::move(_2));
        else
            Become(std::move(_1 %= _2));
    }
    if (Is<Modulo>()) {
        hash = _1.Hash() ^ _2.Hash();
    }
}
