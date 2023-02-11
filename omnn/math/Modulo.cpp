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
    return out << " % ";
}

std::ostream& Modulo::code_sign(std::ostream& out) const
{
	return out << '%';
}

std::ostream& omnn::math::Modulo::code(std::ostream& out) const {
    auto _1stIsInt = _1.IsInt();
    if (_1stIsInt)
        out << '(';
    else
		out << "((uint)(";
    PrintCodeVal(out, _1);
    if (!_1stIsInt)
	    out << ')';
    code_sign(out);
    PrintCodeVal(out, _2);
    out << ')';
    return out;
}

max_exp_t omnn::math::Modulo::getMaxVaExp(const Valuable& _1, const Valuable& _2) {
	return _1.getMaxVaExp();
}

void Modulo::optimize() {
    _1.optimize();
    _2.optimize();
    if (_1.IsModulo()) {
        auto& m1 = _1.as<Modulo>();
        auto& m1devisor = m1.get2();
        if ((m1devisor.IsInt() && _2.IsInt() && m1devisor.ca() <= _2.ca())
			|| m1devisor == _2)
		{
            Become(std::move(_1));
            return;
        }
    } else if (_2.IsInt()) {
        if (_2 == constants::zero) {
			IMPLEMENT
            Become(std::move(_1));
        }
        else if (_1.IsInt()) {
            if (_2 == constants::one)
                Become(0_v);
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
