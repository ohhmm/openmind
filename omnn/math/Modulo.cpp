/*
 * Modulo.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#include <omnn/math/Modulo.h>
#include <omnn/math/Variable.h>

using namespace omnn::math;


Valuable::vars_cont_t Modulo::VarsForCommoning;

std::ostream& Modulo::print_sign(std::ostream& out) const
{
    return out << " mod ";
}

void Modulo::optimize(){
    _1.optimize();
    _2.optimize();
    if (_2.IsInt()) {
        if (_2 == constants::zero) {
			IMPLEMENT
            Become(std::move(_1));
        }
        else if (_1.IsInt()) {
            if (_2 == constants::one)
                Become(std::move(_2));
            else
                Become(std::move(_1 %= _2));
        }
    }
    if (IsModulo()) {
        hash = _1.Hash() ^ _2.Hash();
    }
}

Valuable Modulo::operator-() const
{
    auto copy = *this;
    copy.update1(-_1);
    return copy;
}

bool Modulo::IsComesBefore(const Valuable& v) const
{
    auto is = v.IsModulo();
    if (is) {
        auto& vModulo = v.as<Modulo>();
        auto& v_1st = vModulo.get1();
        is = _1.IsComesBefore(v_1st);
        if (!is)
		{
            is = _1 == v_1st
				&& vModulo.get2().IsComesBefore(_2);
        }
    }
    return is;
}
