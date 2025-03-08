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

    CHECK_OPTIMIZATION_CACHE
    if (_1.IsModulo()) {
        auto& m1 = _1.as<Modulo>();
        auto& m1devisor = m1.get2();
        CHECK_OPTIMIZATION_CACHE
        if (m1devisor == _2)
		{
            CHECK_OPTIMIZATION_CACHE
            Become(std::move(_1));
        } else if (m1devisor.IsInt() && _2.IsInt()) {
            if (m1devisor < _2) {
                CHECK_OPTIMIZATION_CACHE
                Become(std::move(_1));
            } else if (m1devisor > _2) {
                CHECK_OPTIMIZATION_CACHE
                m1.update2(std::move(_2));
                CHECK_OPTIMIZATION_CACHE
                Become(std::move(_1));
            }
        }
    } else if (_2.IsInt()) {
        if (_2.IsZero()) {
			// FIXME: upstream math theory for the remainder of division by zero (x mod 0)
			// TODO : keeping this makes IntMod ops work 
			//IMPLEMENT
            Become(std::move(_1));
        }
        else if (_1.IsInt()) {
            CHECK_OPTIMIZATION_CACHE
            if (_2 == constants::one)
                Become(0_v);
            else {
                CHECK_OPTIMIZATION_CACHE
                Become(std::move(_1 %= _2));
            }
        } else if (_1.IsSimple() && _1.IsRational() == YesNoMaybe::Yes) {
            auto rational_a = static_cast<a_rational>(_1);
            auto denominator = boost::multiprecision::denominator(rational_a);
            auto result = boost::multiprecision::numerator(rational_a) % (_2.ca() * denominator);
            auto modulo = a_rational(result, denominator);
            Become(std::move(modulo));
        }
    }

    CHECK_OPTIMIZATION_CACHE
    if (IsModulo()) {
        hash = _1.Hash() ^ _2.Hash();
        MarkAsOptimized();
        STORE_TO_CACHE
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

std::pair<bool, Valuable> Modulo::IsModSimplifiable(const Valuable& v) const 
{
    // Check if divisor is x^m
    if (v.IsExponentiation()) {
        auto& exp = v.as<Exponentiation>();
        auto& base = exp.getBase();
        auto divisor_exp = exp.getExponentiation();

        // Handle direct exponentiation case first
        if (_1.IsExponentiation()) {
            auto& dividend_exp = _1.as<Exponentiation>();
            if (dividend_exp.getBase() == base) {
                auto term_exp = dividend_exp.getExponentiation();
                // If dividend exponent is greater, result is (x+1)
                if (term_exp > divisor_exp) {
                    return {true, base + 1};
                }
                // If exponents are equal, result is 0
                if (term_exp == divisor_exp) {
                    return {true, 0};
                }
            }
        }
        
        // Handle k*(x^n) % (x^m) pattern
        if (_1.IsProduct()) {
            auto& prod = _1.as<Product>();
            
            // First check for k*(x^n) pattern
            for (const auto& term : prod) {
                if (term.IsExponentiation() && 
                    term.as<Exponentiation>().getBase() == base) {
                    auto term_exp = term.as<Exponentiation>().getExponentiation();
                    
                    // Calculate coefficient k and check for (x+1)
                    Valuable k = constants::one;
                    bool found_xplus1 = false;
                    
                    for (const auto& other : prod) {
                        if (other != term) {
                            if (other == base + 1) {
                                found_xplus1 = true;
                            } else {
                                k *= other;
                            }
                        }
                    }
                    
                    // Case 1: k*(x^n) % (x^n) = 0 for any k without (x+1)
                    if (term_exp == divisor_exp && !found_xplus1) {
                        return {true, 0};
                    }
                    
                    // Case 2: (x^n * (x+1)) % (x^n) = x+1
                    if (term_exp == divisor_exp && found_xplus1) {
                        return {true, base + 1};
                    }
                    
                    // Case 3: If dividend exponent > divisor exponent, result is (x+1)
                    if (term_exp > divisor_exp) {
                        return {true, base + 1};
                    }
                    
                    // Case 4: k*(x^n) % x = k % x when n = 1
                    if (divisor_exp.IsInt() && divisor_exp.ca() == 1) {
                        if (found_xplus1) {
                            k *= (base + 1);
                        }
                        return {true, k % base};
                    }
                }
            }
        } else if (_1.IsExponentiation() && 
                   _1.as<Exponentiation>().getBase() == base) {
            // Handle x^n % x^m pattern
            auto dividend_exp = _1.as<Exponentiation>().getExponentiation();
            if (dividend_exp == divisor_exp) {
                return {true, 0};
            }
        }
    }
    return {false, {}};
}
