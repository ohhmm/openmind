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

void Modulo::optimize() {
    DUO_OPT_PFX

    if (_1 == _2) {
        Become(0);
        return;
    }

    DUO_USE_CACHE(DbModuloOptimizationCache)

    CHECK_OPTIMIZATION_CACHE
    _1.optimize();
    CHECK_OPTIMIZATION_CACHE
    _2.optimize();

    if (_1 == _2) {
        Become(0);
        return;
    }

    static thread_local bool optimizing_product = false;

    if (_1.IsProduct() && !optimizing_product) {
        OptimizeOff opt_off;
        optimizing_product = true;

        try {
            const auto& product = _1.as<Product>();
            if (product.GetConstCont().size() == 2) {
                auto it = product.GetConstCont().begin();
                const auto& first = *it++;
                const auto& second = *it;

                _1 = Modulo(
                    Modulo(
                        Modulo(
                            Modulo(
                                Modulo(
                                    Product({
                                        Modulo(first, _2),    // first is 2_v
                                        Modulo(second, _2)    // second is v1
                                    }),
                                    _2
                                ),
                                _2
                            ),
                            _2
                        ),
                        _2
                    ),
                    _2
                );

                hash = _1.Hash() ^ _2.Hash();
                MarkAsOptimized();
                STORE_TO_CACHE
            }
        } catch (...) {
            optimizing_product = false;
            throw;
        }

        optimizing_product = false;
        return;
    }

    if (_1.IsModulo()) {
        auto& m1 = _1.as<Modulo>();
        auto& m1devisor = m1.get2();

        if (m1devisor == _2) {
            Become(std::move(_1));
        } else if (m1devisor.IsInt() && _2.IsInt()) {
            if (m1devisor < _2) {
                Become(std::move(_1));
            } else if (m1devisor > _2) {
                m1.update2(std::move(_2));
                Become(std::move(_1));
            }
        }
    } else if (_2.IsInt()) {
        if (_2.IsZero()) {
        // FIXME: upstream math theory for the remainder of division by zero (x mod 0)
        // TODO : keeping this makes IntMod ops work
        //IMPLEMENT
            Become(std::move(_1));
        } else if (_1.IsInt()) {
            if (_2 == constants::one)
                Become(0_v);
            else {
                Become(std::move(_1 %= _2));
            }
        }
    }

    hash = _1.Hash() ^ _2.Hash();
    MarkAsOptimized();
    STORE_TO_CACHE
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
