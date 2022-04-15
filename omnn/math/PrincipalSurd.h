#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn::math {

/// This implementation objects kept if it is surd, otherwise it optimized out.
class PrincipalSurd /// Also known as principal root of Nth index
        : public DuoValDescendant<PrincipalSurd>
{
    using base = DuoValDescendant<PrincipalSurd>;
public:
    using base::base;
    PrincipalSurd(const Valuable& radicand, const Valuable& index = 2);

protected:
    std::ostream &print_sign(std::ostream &out) const override;
};

}
