/*
 * Modulo.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#include <omnn/math/Modulo.h>

#include <omnn/math/Exponentiation.h>
#include <omnn/math/Fraction.h>
#include <omnn/math/Product.h>
#include <omnn/math/Variable.h>


using namespace omnn::math;

namespace {
CACHE(DbModuloOptimizationCache);
}

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

Valuable& Modulo::optimize() {
    this->MarkAsOptimized();
    return *this;
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

bool Modulo::IsComesBefore(const Modulo& mod) const {
    auto& modDividend = mod.getDividend();
    auto equalDividends = getDividend() == modDividend;
    if (equalDividends) {
        return getDevisor().IsComesBefore(mod.getDevisor());
    }
    auto equalDivisors = getDevisor() == mod.getDevisor();
    if (equalDivisors) {
        return getDividend().IsComesBefore(modDividend);
    }
    auto is = _1.IsComesBefore(modDividend);
    if (!is) {
        is = _1 == modDividend && mod.get2().IsComesBefore(_2);
    }
    return is;
}

bool Modulo::IsComesBefore(const Valuable& v) const
{
    auto is = v.IsModulo();
    if (is) {
        is = IsComesBefore(v.as<Modulo>());
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
