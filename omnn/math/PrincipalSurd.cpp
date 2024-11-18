#include "PrincipalSurd.h"
#include "Product.h"

using namespace omnn::math;


std::ostream& PrincipalSurd::print_sign(std::ostream &out) const
{
    out << 'r';
    return out;
}

std::ostream& PrincipalSurd::print(std::ostream& out) const {
    if (_2 == 2)
        out << "sqrt(" << _1 << ')';
    else
        out << '(' << _2 << 'r' << _1 << ')';
    return out;
}

std::ostream& PrincipalSurd::code(std::ostream& out) const {
    if(_2 == 2)
        out << "sqrt(" << _1 << ')';
    else
        out << "pow(" << _1 << ", 1/(" << _2 << "))";
    return out;
}

std::pair<bool, Valuable> PrincipalSurd::IsSummationSimplifiable(const Valuable& v) const {
    std::pair<bool, Valuable> is;
    is.first = operator==(v);
    if (is.first) {
        is.second = v * 2;
    }
    return is;
}

std::pair<bool, Valuable> PrincipalSurd::IsMultiplicationSimplifiable(const Valuable& v) const {
    std::pair<bool, Valuable> is;
    is.first = v.IsRadical();
    if (is.first) {
        auto& surd = v.as<PrincipalSurd>();
        is.first = _2 == surd._2;
        if (is.first) {
            if (_1 == surd._1) {
                if (_2 == constants::two)
                    is.second = _1;
                else
                    is.second = PrincipalSurd{_1, _2 / 2};
            } else {
                is.second = PrincipalSurd{_1 * surd._1, _2};
            }
        } 
    }
    return is;
}

void PrincipalSurd::optimize() {
    if (!optimizations || is_optimized())
        return;

    _1.optimize();
    _2.optimize();

    if (index() == constants::one || IsEquation()) {
        Become(std::move(radicand()));
        return;
    }

    if (_1.IsInt()) {
        auto& rdcnd = _1.ca();
        if (rdcnd < 0) {
            Become(Product{constants::i, {PrincipalSurd{-_1, _2}}});
            return;
        } else if (_1.IsZero() || _2 == constants::one) {
            Become(std::move(_1));
            return;
        } else {
            auto& index = Index();
            if (index.IsInt()) {
                if (index == constants::one) {
                    Become(std::move(_1));
                    return;
                }
                auto& idx = Index().ca();
                if (idx == 0) {
                    IMPLEMENT
                }
                while (_2.bit().IsZero()) {
                    auto newRadicand = _1.Sqrt();
                    if (newRadicand.IsPrincipalSurd())
                        break;
                    else {
                        update1(newRadicand);
                        update2(_2.shr());
                    }
                }
            }
        }
    }

    if(index()==constants::one || IsEquation()){
        Become(std::move(radicand()));
        return;
    }

    if (IsPrincipalSurd()) {
        MarkAsOptimized();
    }
}

Valuable& PrincipalSurd::sq() {
    index() /= 2;
    optimized = {};
    optimize();
    return *this;
}

Valuable PrincipalSurd::Sqrt() const {
    return PrincipalSurd{_1, _2 * 2};
}

Valuable PrincipalSurd::Sign() const {
    return constants::one; // See https://github.com/ohhmm/openmind/blob/011305e988292473919a523736cf6e913dbb55ef/omnn/math/i.cpp#L66
}

PrincipalSurd::operator double() const {
    auto d = static_cast<double>(_1);
    return _2 == 2 ? std::sqrt(d) : std::pow(d, 1./static_cast<double>(_2));
}

PrincipalSurd::operator long double() const {
    auto d = static_cast<long double>(_1);
    return _2 == 2 ? std::sqrt(d) : std::pow(d, 1./static_cast<long double>(_2));
}

Valuable::vars_cont_t PrincipalSurd::GetVaExps() const {
    auto vaExps = _1.GetVaExps();
    if(_2.IsInt() && _2 > constants::zero) {
        if (_2 != constants::one)
            for (auto& ve : vaExps)
                ve.second /= _2;
    } else if (!FindVa()) {
        vaExps.clear();
    } else if (_2.IsSimpleFraction() && _2 > constants::zero) {
        for (auto& ve : vaExps)
            ve.second /= _2;
    } else {
        IMPLEMENT
    }
    return vaExps;
}

max_exp_t PrincipalSurd::getMaxVaExp(const Valuable& _1, const Valuable& _2) {
    auto exponentiation = _1.getMaxVaExp();
    exponentiation /= static_cast<decltype(exponentiation)>(_2);
    return exponentiation;
}

bool PrincipalSurd::operator <(const Valuable& other) const {

    Valuable power;
    if(other.IsPrincipalSurd()) {
        auto& ps = other.as<PrincipalSurd>();
        if(_2 == ps._2) {
            return _1 < ps._1;
        } else if (_1 == ps._1) {
            return ps._2 < _2;
        } else if (_2.IsInt() && ps._2.IsInt()) {
            power = std::max(_2, ps._2);
        } else {
            power = _2 * ps._2;
        }
    } else if(_2 == constants::one) {
        return other < _1;
    } else {
        power = _2;
    }

    OptimizeOn on;
    return (*this ^ power) < (other ^ power);
}

Valuable& PrincipalSurd::operator^=(const Valuable& e) {
    if (_2 == e)
        return _1;
    else
        return base::operator^=(e);
}

bool PrincipalSurd::IsComesBefore(const Valuable& v) const {
    if(v.IsPrincipalSurd()) {
        auto& ps = v.as<PrincipalSurd>();
        if (_2 == ps._2) {
            return _1.IsComesBefore(ps._1);
        } else {
            return _2 < ps._2;
        }
    } else {
        return base::IsComesBefore(v);
    }
}

Valuable PrincipalSurd::InCommonWith(const PrincipalSurd& surd) const {
    if (_2 == surd._2) {
        auto ic = _1.InCommonWith(surd._1);
        if (ic.IsInt()) {
            if (ic == 1)
                return 1;
            else
                return PrincipalSurd{ic, _2};
        } else {
            return PrincipalSurd{ic, _2};
        }
    } else if (_1 == surd._1) {
        auto& mindex = std::min(_2, surd._2);
        auto& maxdex = std::max(_2, surd._2);
        if ((maxdex / mindex).IsInt()) {
            return PrincipalSurd{_1, maxdex};
        }
    }
    else {
        auto gcdRadicands = Radicand().GCD(surd.Radicand());
        if (gcdRadicands == constants::one) {
            return gcdRadicands;
        }
        LOG_AND_IMPLEMENT(*this << " InCommonWith " << surd);
    }
    return constants::one;
}

Valuable PrincipalSurd::InCommonWith(const Valuable& v) const {
    if(v.IsPrincipalSurd()) {
        return InCommonWith(v.as<PrincipalSurd>());
    } else if(v.Is_i()) {
        return _2.IsEven() == YesNoMaybe::Yes && _1.Sign() != constants::one
            ? v
            : constants::one;
    } else if (v.IsVa() && !HasVa(v.as<Variable>())){
        return constants::one;
    } else {
        auto commonUnderRoot = _1.InCommonWith(v ^ _2);
        if(commonUnderRoot == constants::one) {
            return commonUnderRoot;
        } else {
            return PrincipalSurd{commonUnderRoot, _2};
        }
    }
}

const Valuable::vars_cont_t& PrincipalSurd::getCommonVars() const {
    return commonVars;
}

void PrincipalSurd::solve(const Variable& va, solutions_t& solutions) const {
    Radicand().solve(va, solutions);
}
