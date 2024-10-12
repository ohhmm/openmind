/*
 * Modulo.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#include <omnn/math/Modulo.h>
#include <omnn/math/Variable.h>
#include <omnn/math/Exponentiation.h>
#include <omnn/math/Fraction.h>
#include <omnn/math/Product.h>


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

std::ostream& Modulo::code(std::ostream& out) const {
    auto isInt = _1.IsInt();
    if (isInt)
        out << '(';
    else
		out << "((uint)(";
    PrintCodeVal(out, _1);
    if (!isInt)
	    out << ')';
    code_sign(out);
    isInt = _2.IsInt();
    if (!isInt)
        out << "((uint)(";
    PrintCodeVal(out, _2);
    if (isInt)
        out << ')';
    else
        out << ")))";
    return out;
}

max_exp_t Modulo::getMaxVaExp(const Valuable& _1, const Valuable& _2) {
	return _1.getMaxVaExp();
}

bool Modulo::operator==(const Modulo& modulo) const {
    return base::operator ==(modulo);
}

void Modulo::optimize() {
    DUO_OPT_PFX
    _1.optimize();
    _2.optimize();
    if (_1.IsModulo()) {
        auto& m1 = _1.as<Modulo>();
        auto& m1devisor = m1.get2();
        if (m1devisor == _2)
		{
            Become(std::move(_1));
            return;
        } else if (m1devisor.IsInt() && _2.IsInt()) {
            if (m1devisor < _2)
				Become(std::move(_1));
            else if (m1devisor > _2) {
                m1.update2(std::move(_2));
                Become(std::move(_1));
            }
            return;
        }
    } else if (_2.IsInt()) {
        if (_2.IsZero()) {
			// FIXME: upstream math theory for the remainder of division by zero (x mod 0)
			// TODO : keeping this makes IntMod ops work 
			//IMPLEMENT
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

Valuable& Modulo::sq() {
	return operator^=(2_v);
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

Valuable::vars_cont_t Modulo::GetVaExps() const {
    auto vars = _1.GetVaExps();
    for (auto& r : _2.GetVaExps()) {
        vars[r.first] -= r.second;
    }
    return vars;
}
