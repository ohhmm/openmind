#include "PrincipalSurd.h"
#include "Product.h"

using namespace omnn::math;


PrincipalSurd::PrincipalSurd(const Valuable& radicand, const Valuable& index)
    : base(radicand, index)
{

}

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
    if (_1.IsInt()) {
        auto& rdcnd = _1.ca();
        if (rdcnd < 0) {
            Become(Product{constants::i, {PrincipalSurd{-_1, _2}}});
            return;
        } else if (rdcnd == 0) {
            Become(0);
            return;
        } else if (Index().IsInt()) {
            auto& idx = Index().ca(); 
            if (idx == 0) {
                IMPLEMENT
            }
            while (_2.bit() == constants::zero) {
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

    if(index()==constants::one){
        Become(std::move(radicand()));
        return;
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
    if(Radicand().IsInt())
        return Radicand().Sign();
    else{
        LOG_AND_IMPLEMENT(*this << " . Sign()");
    }
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
    if(_2.IsInt() && _2 > constants::one) {
        for(auto& ve : vaExps)
            ve.second /= _2;
    } else {
        IMPLEMENT
    }
    return vaExps;
}

max_exp_t PrincipalSurd::getMaxVaExp(const Valuable& _1, const Valuable& _2) {
    return _1.getMaxVaExp() / _2.ca();
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
    } else {
        power = _2;
    }

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

Valuable PrincipalSurd::InCommonWith(const Valuable& v) const {
    if(v.IsPrincipalSurd()) {
        auto& ps = v.as<PrincipalSurd>();
        if (_2 == ps._2) {
            auto ic = _1.InCommonWith(ps._1);
            if(ic.IsInt()) {
                if(ic == 1)
                    return 1;
                else
                    return PrincipalSurd{ic, _2};
            } else {
                return PrincipalSurd{ic, _2};
            }
        } else {
            IMPLEMENT
        }
    } else if(v.Is_i()) {
        return _2.IsEven() == YesNoMaybe::Yes && _1.Sign() != constants::one
            ? v
            : constants::one;
    } else {
        return base::InCommonWith(v);
    }
}
